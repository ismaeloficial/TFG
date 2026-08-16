#set page(width: 27cm, height: auto, margin: (x: 1.2cm, y: 1cm))
#set text(font: "New Computer Modern", size: 10pt, lang: "es")

#align(center)[#text(size: 14pt, weight: "bold")[2.2.9. Planificación temporal — Diagrama de Gantt]]
#v(0.6em)
#text(size: 9pt, style: "italic")[
  El tramo entre finales de mayo y el 19 de junio de 2026 se dedicó al aprendizaje, la revisión de antecedentes y la definición de objetivos (capítulos 3 y 4 de esta memoria), al margen del presupuesto de horas de las fases técnicas. A partir del 20 de junio, el diagrama cubre las fases 0-7 y el margen de contingencia definidos en el plan de desarrollo (≈270h), agrupadas en bloques de dos semanas, hasta el 10 de septiembre de 2026.
]
#v(1.2em)

#let cDone = rgb("#4a7c59")
#let cProgress = rgb("#3d6ea5")
#let cPlanned = rgb("#d99a3d")
#let cBuffer = rgb("#b5b5b5")

#let cPrevio = rgb("#7a9e8e")

#let rows = (
  (name: "Aprendizaje, antecedentes y objetivos", occ: (1,1,0,0,0,0,0,0), color: cPrevio),
  (name: "Fase 0 — Entorno y esqueleto (15h)", occ: (0,0,1,0,0,0,0,0), color: cDone),
  (name: "Fase 1 — Captura RFID (30h)", occ: (0,0,0,1,0,0,0,0), color: cDone),
  (name: "Fase 2 — Backend y persistencia (35h)", occ: (0,0,0,0,1,0,0,0), color: cDone),
  (name: "Fase 3 — Robustez ante desconexión (25h)", occ: (0,0,0,0,0,1,0,0), color: cDone),
  (name: "Fase 7 — Documentación (60h)", occ: (0,0,1,1,1,1,1,1), color: cProgress),
  (name: "Fase 4 — Lectura QR (35h)", occ: (0,0,0,0,0,0,1,0), color: cPlanned),
  (name: "Fase 5 — Integración física (20h)", occ: (0,0,0,0,0,0,1,0), color: cPlanned),
  (name: "Fase 6 — Validación funcional (30h)", occ: (0,0,0,0,0,0,0,1), color: cPlanned),
  (name: "Margen de contingencia (20h)", occ: (0,0,0,0,0,0,0,1), color: cBuffer),
)

#let weekLabels = (
  [B1 #linebreak() 23 may–5 jun],
  [B2 #linebreak() 6–19 jun],
  [B3 #linebreak() 20 jun–3 jul],
  [B4 #linebreak() 4–17 jul],
  [B5 #linebreak() 18–31 jul],
  [B6 #linebreak() 1–14 ago],
  [B7 #linebreak() 15–28 ago],
  [B8 #linebreak() 29 ago–10 sep],
)

#table(
  columns: (6.4cm,) + (1fr,)*8,
  align: center + horizon,
  stroke: 0.5pt + gray,
  inset: 5pt,
  table.header(
    [*Fase*],
    ..weekLabels.map(w => text(size: 8pt, weight: "bold")[#w])
  ),
  ..rows.map(r => (
    text(size: 8.5pt)[#r.name],
    ..r.occ.map(o => if o == 1 { table.cell(fill: r.color)[] } else { [] })
  )).flatten()
)

#v(1.2em)
#grid(
  columns: 4,
  column-gutter: 1.8em,
  row-gutter: 0.6em,
  align: horizon,
  box(width: 0.9cm, height: 0.45cm, fill: cPrevio), [#text(size: 9pt)[Aprendizaje, antecedentes y objetivos]],
  box(width: 0.9cm, height: 0.45cm, fill: cDone), [#text(size: 9pt)[Completado y verificado]],
  box(width: 0.9cm, height: 0.45cm, fill: cProgress), [#text(size: 9pt)[En curso]],
  box(width: 0.9cm, height: 0.45cm, fill: cPlanned), [#text(size: 9pt)[Planificado]],
  box(width: 0.9cm, height: 0.45cm, fill: cBuffer), [#text(size: 9pt)[Margen de contingencia]], [], [],
)
#v(0.4em)
#text(size: 8pt, style: "italic")[
  Estado a 13 de agosto de 2026 (bloque B6). Total: 270h repartidas entre las fases 0-7 y el margen de contingencia, dentro del límite normativo de 300h. La Fase 4 queda bloqueada hasta la llegada del módulo QR de repuesto (mínimo 18 de agosto); la Fase 7 se adelanta en paralelo mientras tanto, con un ritmo más gradual que deja días libres entre sesiones de trabajo.
]
