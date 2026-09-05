import { useEffect, useMemo, useState } from "react";
import { getHistorial } from "../api";
import GraficaFrutas from "./GraficaFrutas";
import GraficaLecturasPorDia from "./GraficaLecturasPorDia";

const REFRESH_MS = 4000;

const FILTROS_VACIOS = {
  tipoFruta: "",
  codigo: "",
  procedencia: "",
  pesoMin: "",
  pesoMax: "",
  desde: "",
  hasta: "",
};

function formatDate(iso) {
  return new Date(iso).toLocaleString("es-ES");
}

function valoresUnicos(fichas, campo) {
  return [...new Set(fichas.map((f) => f[campo]).filter(Boolean))].sort();
}

export default function HistorialTable({ fichas = [], fichasByCode }) {
  const [historial, setHistorial] = useState([]);
  const [error, setError] = useState(null);
  const [modalAbierto, setModalAbierto] = useState(false);
  const [filtrosAplicados, setFiltrosAplicados] = useState(FILTROS_VACIOS);
  const [filtrosBorrador, setFiltrosBorrador] = useState(FILTROS_VACIOS);

  useEffect(() => {
    let cancelled = false;

    async function load() {
      try {
        const data = await getHistorial();
        if (!cancelled) {
          setHistorial(data);
          setError(null);
        }
      } catch {
        if (!cancelled) setError("No se pudo conectar con el backend (¿está arrancado?)");
      }
    }

    load();
    const interval = setInterval(load, REFRESH_MS);
    return () => {
      cancelled = true;
      clearInterval(interval);
    };
  }, []);

  const tiposFruta = useMemo(() => valoresUnicos(fichas, "tipo_fruta"), [fichas]);
  const procedencias = useMemo(() => valoresUnicos(fichas, "procedencia"), [fichas]);

  function abrirModal() {
    setFiltrosBorrador(filtrosAplicados);
    setModalAbierto(true);
  }

  function actualizarBorrador(campo, valor) {
    setFiltrosBorrador((prev) => ({ ...prev, [campo]: valor }));
  }

  function aplicarFiltros() {
    setFiltrosAplicados(filtrosBorrador);
    setModalAbierto(false);
  }

  function limpiarFiltros() {
    setFiltrosBorrador(FILTROS_VACIOS);
    setFiltrosAplicados(FILTROS_VACIOS);
  }

  const filtrosActivos = Object.values(filtrosAplicados).filter((v) => v !== "").length;

  const historialFiltrado = useMemo(() => {
    return historial.filter((entry) => {
      const ficha = fichasByCode?.[entry.raw_value];
      const f = filtrosAplicados;

      if (f.tipoFruta && ficha?.tipo_fruta !== f.tipoFruta) return false;
      if (f.procedencia && ficha?.procedencia !== f.procedencia) return false;
      if (f.codigo && !entry.raw_value.toLowerCase().includes(f.codigo.trim().toLowerCase())) return false;
      if (f.pesoMin !== "" && !(ficha?.peso_kg != null && Number(ficha.peso_kg) >= Number(f.pesoMin))) return false;
      if (f.pesoMax !== "" && !(ficha?.peso_kg != null && Number(ficha.peso_kg) <= Number(f.pesoMax))) return false;
      if (f.desde && new Date(entry.read_at) < new Date(f.desde)) return false;
      if (f.hasta && new Date(entry.read_at) > new Date(f.hasta)) return false;

      return true;
    });
  }, [historial, fichasByCode, filtrosAplicados]);

  if (error) return <p className="error">{error}</p>;

  return (
    <div className="historial-layout">
      <div className="panel historial-panel">
        <div className="panel-header">
          <div>
            <h2>Historial de lecturas</h2>
            <p className="hint">Se actualiza solo cada {REFRESH_MS / 1000}s.</p>
          </div>

          <button
            className={`filtro-btn ${filtrosActivos > 0 ? "filtro-btn-activo" : ""}`}
            onClick={abrirModal}
          >
            <span className="filtro-icono">⚙</span> Filtros{filtrosActivos > 0 ? ` (${filtrosActivos})` : ""}
          </button>
        </div>

        <div className="tabla-scroll">
          <table>
            <thead>
              <tr>
                <th>Tipo</th>
                <th>Valor leído</th>
                <th>Ficha de caja</th>
                <th>Punto de lectura</th>
                <th>Leído</th>
              </tr>
            </thead>
            <tbody>
              {historialFiltrado.map((entry) => {
                const ficha = fichasByCode?.[entry.raw_value];
                return (
                  <tr key={entry.id}>
                    <td>
                      <span className={`badge badge-${entry.read_type.toLowerCase()}`}>{entry.read_type}</span>
                    </td>
                    <td className="mono">{entry.raw_value}</td>
                    <td>
                      {ficha ? `${ficha.tipo_fruta}${ficha.variedad ? " · " + ficha.variedad : ""}` : "— sin asociar —"}
                    </td>
                    <td>{entry.read_point ?? "—"}</td>
                    <td>{formatDate(entry.read_at)}</td>
                  </tr>
                );
              })}
              {historialFiltrado.length === 0 && (
                <tr>
                  <td colSpan={5} className="empty">
                    {filtrosActivos > 0 ? "Sin lecturas para estos filtros." : "Todavía no hay lecturas. Acerca una tag al lector."}
                  </td>
                </tr>
              )}
            </tbody>
          </table>
        </div>
      </div>

      <div className="graficas-columna">
        <div className="panel grafica-panel">
          <h3>Lecturas por tipo de fruta</h3>
          <div className="grafica-contenido">
            <GraficaFrutas historial={historial} fichasByCode={fichasByCode} />
          </div>
        </div>
        <div className="panel grafica-panel">
          <h3>Lecturas por día / mes / año</h3>
          <GraficaLecturasPorDia historial={historial} />
        </div>
      </div>

      {modalAbierto && (
        <div className="modal-backdrop" onClick={() => setModalAbierto(false)}>
          <div className="modal-filtros" onClick={(e) => e.stopPropagation()}>
            <div className="modal-filtros-header">
              <h3>Filtrar historial</h3>
              <button className="modal-cerrar" onClick={() => setModalAbierto(false)} aria-label="Cerrar">
                ×
              </button>
            </div>

            <div className="modal-filtros-body">
              <div className="filtro-campo">
                <label>Tipo de fruta</label>
                <select
                  value={filtrosBorrador.tipoFruta}
                  onChange={(e) => actualizarBorrador("tipoFruta", e.target.value)}
                >
                  <option value="">Todas</option>
                  {tiposFruta.map((t) => (
                    <option key={t} value={t}>
                      {t}
                    </option>
                  ))}
                </select>
              </div>

              <div className="filtro-campo">
                <label>Procedencia</label>
                <select
                  value={filtrosBorrador.procedencia}
                  onChange={(e) => actualizarBorrador("procedencia", e.target.value)}
                >
                  <option value="">Todas</option>
                  {procedencias.map((p) => (
                    <option key={p} value={p}>
                      {p}
                    </option>
                  ))}
                </select>
              </div>

              <div className="filtro-campo filtro-campo-ancho">
                <label>Código</label>
                <input
                  type="text"
                  placeholder="p. ej. FDDF3B"
                  value={filtrosBorrador.codigo}
                  onChange={(e) => actualizarBorrador("codigo", e.target.value)}
                />
              </div>

              <div className="filtro-campo">
                <label>Peso mínimo (kg)</label>
                <input
                  type="number"
                  min="0"
                  step="0.1"
                  placeholder="0"
                  value={filtrosBorrador.pesoMin}
                  onChange={(e) => actualizarBorrador("pesoMin", e.target.value)}
                />
              </div>

              <div className="filtro-campo">
                <label>Peso máximo (kg)</label>
                <input
                  type="number"
                  min="0"
                  step="0.1"
                  placeholder="Sin límite"
                  value={filtrosBorrador.pesoMax}
                  onChange={(e) => actualizarBorrador("pesoMax", e.target.value)}
                />
              </div>

              <div className="filtro-campo">
                <label>Desde (fecha y hora)</label>
                <input
                  type="datetime-local"
                  value={filtrosBorrador.desde}
                  onChange={(e) => actualizarBorrador("desde", e.target.value)}
                />
              </div>

              <div className="filtro-campo">
                <label>Hasta (fecha y hora)</label>
                <input
                  type="datetime-local"
                  value={filtrosBorrador.hasta}
                  onChange={(e) => actualizarBorrador("hasta", e.target.value)}
                />
              </div>
            </div>

            <div className="modal-filtros-footer">
              <button className="filtro-limpiar" onClick={limpiarFiltros}>
                Limpiar filtros
              </button>
              <button className="filtro-aplicar" onClick={aplicarFiltros}>
                Aplicar filtros
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
