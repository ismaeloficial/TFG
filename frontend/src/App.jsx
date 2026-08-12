import { useCallback, useEffect, useMemo, useState } from "react";
import "./App.css";
import FichasCajaPanel from "./components/FichasCajaPanel";
import HistorialTable from "./components/HistorialTable";
import { getFichasCaja } from "./api";

export default function App() {
  const [fichas, setFichas] = useState([]);
  const [tab, setTab] = useState("historial");

  const refreshFichas = useCallback(() => {
    getFichasCaja()
      .then(setFichas)
      .catch(() => {});
  }, []);

  useEffect(() => {
    refreshFichas();
  }, [refreshFichas]);

  const fichasByCode = useMemo(() => Object.fromEntries(fichas.map((f) => [f.code, f])), [fichas]);

  return (
    <div className="app">
      <header>
        <h1>Trazabilidad de cajas de fruta</h1>
        <p className="subtitle">TFG — panel de seguimiento</p>
      </header>

      <nav className="tabs">
        <button className={tab === "historial" ? "active" : ""} onClick={() => setTab("historial")}>
          Historial
        </button>
        <button className={tab === "fichas" ? "active" : ""} onClick={() => setTab("fichas")}>
          Fichas de caja
        </button>
      </nav>

      {tab === "historial" && <HistorialTable fichasByCode={fichasByCode} />}
      {tab === "fichas" && <FichasCajaPanel fichas={fichas} onChanged={refreshFichas} />}
    </div>
  );
}
