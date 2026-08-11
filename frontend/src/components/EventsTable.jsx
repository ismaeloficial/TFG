import { useEffect, useState } from "react";
import { getEvents } from "../api";

const REFRESH_MS = 4000;

function formatDate(iso) {
  return new Date(iso).toLocaleString("es-ES");
}

export default function EventsTable({ boxesByCode }) {
  const [events, setEvents] = useState([]);
  const [error, setError] = useState(null);

  useEffect(() => {
    let cancelled = false;

    async function load() {
      try {
        const data = await getEvents();
        if (!cancelled) {
          setEvents(data);
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
      <h2>Eventos de lectura</h2>
      <p className="hint">Se actualiza solo cada {REFRESH_MS / 1000}s.</p>
      <table>
        <thead>
          <tr>
            <th>Tipo</th>
            <th>Valor leído</th>
            <th>Caja</th>
            <th>Punto de lectura</th>
            <th>Leído</th>
          </tr>
        </thead>
        <tbody>
          {events.map((ev) => {
            const box = boxesByCode?.[ev.raw_value];
            return (
              <tr key={ev.id}>
                <td>
                  <span className={`badge badge-${ev.read_type.toLowerCase()}`}>{ev.read_type}</span>
                </td>
                <td className="mono">{ev.raw_value}</td>
                <td>{box ? `${box.tipo_fruta}${box.variedad ? " · " + box.variedad : ""}` : "— sin asociar —"}</td>
                <td>{ev.read_point ?? "—"}</td>
                <td>{formatDate(ev.read_at)}</td>
              </tr>
            );
          })}
          {events.length === 0 && (
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
