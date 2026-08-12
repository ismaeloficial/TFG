import { useEffect, useState } from "react";
import { getHistorial } from "../api";

const REFRESH_MS = 4000;

function formatDate(iso) {
  return new Date(iso).toLocaleString("es-ES");
}

export default function HistorialTable({ fichasByCode }) {
  const [historial, setHistorial] = useState([]);
  const [error, setError] = useState(null);

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

  if (error) return <p className="error">{error}</p>;

  return (
    <div className="panel">
      <h2>Historial de lecturas</h2>
      <p className="hint">Se actualiza solo cada {REFRESH_MS / 1000}s.</p>
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
          {historial.map((entry) => {
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
          {historial.length === 0 && (
            <tr>
              <td colSpan={5} className="empty">
                Todavía no hay lecturas. Acerca una tag al lector.
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
}
