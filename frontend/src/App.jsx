import { useCallback, useEffect, useMemo, useState } from "react";
import "./App.css";
import BoxesPanel from "./components/BoxesPanel";
import EventsTable from "./components/EventsTable";
import { getBoxes } from "./api";

export default function App() {
  const [boxes, setBoxes] = useState([]);
  const [tab, setTab] = useState("eventos");

  const refreshBoxes = useCallback(() => {
    getBoxes()
      .then(setBoxes)
      .catch(() => {});
  }, []);

  useEffect(() => {
    refreshBoxes();
  }, [refreshBoxes]);

  const boxesByCode = useMemo(() => Object.fromEntries(boxes.map((b) => [b.code, b])), [boxes]);

  return (
    <div className="app">
      <header>
        <h1>Trazabilidad de cajas de fruta</h1>
        <p className="subtitle">TFG — panel de seguimiento</p>
      </header>

      <nav className="tabs">
        <button className={tab === "eventos" ? "active" : ""} onClick={() => setTab("eventos")}>
          Eventos
        </button>
        <button className={tab === "cajas" ? "active" : ""} onClick={() => setTab("cajas")}>
          Cajas
        </button>
      </nav>

      {tab === "eventos" && <EventsTable boxesByCode={boxesByCode} />}
      {tab === "cajas" && <BoxesPanel boxes={boxes} onChanged={refreshBoxes} />}
    </div>
  );
}
