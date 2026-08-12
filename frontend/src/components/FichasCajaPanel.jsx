import { useState } from "react";
import { createFichaCaja, updateFichaCaja } from "../api";

const emptyForm = {
  code: "",
  tipo_fruta: "",
  variedad: "",
  lote: "",
  fecha_recoleccion: "",
  procedencia: "",
  peso_kg: "",
};

function toPayload(form) {
  return {
    ...form,
    variedad: form.variedad || null,
    lote: form.lote || null,
    fecha_recoleccion: form.fecha_recoleccion || null,
    procedencia: form.procedencia || null,
    peso_kg: form.peso_kg === "" ? null : Number(form.peso_kg),
  };
}

export default function FichasCajaPanel({ fichas, onChanged }) {
  const [form, setForm] = useState(emptyForm);
  const [editingId, setEditingId] = useState(null);
  const [error, setError] = useState(null);

  function startEdit(ficha) {
    setEditingId(ficha.id);
    setForm({
      code: ficha.code,
      tipo_fruta: ficha.tipo_fruta,
      variedad: ficha.variedad ?? "",
      lote: ficha.lote ?? "",
      fecha_recoleccion: ficha.fecha_recoleccion ?? "",
      procedencia: ficha.procedencia ?? "",
      peso_kg: ficha.peso_kg ?? "",
    });
  }

  function cancelEdit() {
    setEditingId(null);
    setForm(emptyForm);
  }

  async function handleSubmit(e) {
    e.preventDefault();
    setError(null);
    try {
      if (editingId) {
        await updateFichaCaja(editingId, toPayload(form));
      } else {
        await createFichaCaja(toPayload(form));
      }
      cancelEdit();
      onChanged();
    } catch (err) {
      setError(err.response?.data?.detail ?? "Error guardando la ficha de caja");
    }
  }

  return (
    <div className="panel">
      <h2>Fichas de caja</h2>

      <form onSubmit={handleSubmit} className="box-form">
        <input
          placeholder="Código (ID de tag RFID o texto QR)"
          value={form.code}
          onChange={(e) => setForm({ ...form, code: e.target.value })}
          required
          disabled={!!editingId}
        />
        <input
          placeholder="Tipo de fruta"
          value={form.tipo_fruta}
          onChange={(e) => setForm({ ...form, tipo_fruta: e.target.value })}
          required
        />
        <input
          placeholder="Variedad (opcional)"
          value={form.variedad}
          onChange={(e) => setForm({ ...form, variedad: e.target.value })}
        />
        <input
          placeholder="Lote (opcional)"
          value={form.lote}
          onChange={(e) => setForm({ ...form, lote: e.target.value })}
        />
        <input
          type="date"
          value={form.fecha_recoleccion}
          onChange={(e) => setForm({ ...form, fecha_recoleccion: e.target.value })}
        />
        <input
          placeholder="Procedencia (opcional)"
          value={form.procedencia}
          onChange={(e) => setForm({ ...form, procedencia: e.target.value })}
        />
        <input
          type="number"
          step="0.01"
          placeholder="Peso (kg, opcional)"
          value={form.peso_kg}
          onChange={(e) => setForm({ ...form, peso_kg: e.target.value })}
        />
        <div className="form-actions">
          <button type="submit">{editingId ? "Guardar cambios" : "Dar de alta"}</button>
          {editingId && (
            <button type="button" className="secondary" onClick={cancelEdit}>
              Cancelar
            </button>
          )}
        </div>
      </form>
      {error && <p className="error">{error}</p>}

      <table>
        <thead>
          <tr>
            <th>Código</th>
            <th>Fruta</th>
            <th>Variedad</th>
            <th>Lote</th>
            <th>Procedencia</th>
            <th>Peso (kg)</th>
            <th></th>
          </tr>
        </thead>
        <tbody>
          {fichas.map((ficha) => (
            <tr key={ficha.id}>
              <td className="mono">{ficha.code}</td>
              <td>{ficha.tipo_fruta}</td>
              <td>{ficha.variedad ?? "—"}</td>
              <td>{ficha.lote ?? "—"}</td>
              <td>{ficha.procedencia ?? "—"}</td>
              <td>{ficha.peso_kg ?? "—"}</td>
              <td>
                <button className="secondary" onClick={() => startEdit(ficha)}>
                  Editar
                </button>
              </td>
            </tr>
          ))}
          {fichas.length === 0 && (
            <tr>
              <td colSpan={7} className="empty">
                Todavía no hay fichas de caja dadas de alta.
              </td>
            </tr>
          )}
        </tbody>
      </table>
    </div>
  );
}
