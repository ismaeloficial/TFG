#set page(paper: "a4", margin: (x: 2.5cm, y: 2.5cm), numbering: "1")
#set text(font: "New Computer Modern", size: 11pt, lang: "es")
#set heading(numbering: "1.1.")
#set par(justify: true)

#align(center)[
  #text(size: 20pt, weight: "bold")[Índice de la memoria]

  #v(0.3em)
  #text(size: 13pt)[Sistema embebido de trazabilidad de cajas de fruta]

  #v(0.5em)
  #text(size: 10pt, style: "italic", fill: rgb("#555555"))[
    Documento de trabajo — índice guía para la redacción, sujeto a ajustes
  ]
]

#v(1.5em)

#let nota(txt) = text(size: 9pt, style: "italic", fill: rgb("#8a6d3b"))[ #h(0.4em) (#txt)]
#let unnumbered(txt) = {
  v(0.6em)
  text(size: 13pt, weight: "bold")[#txt]
  v(0.3em)
}

#unnumbered[Índice de figuras]
#unnumbered[Índice de tablas]
#unnumbered[Resumen]
#unnumbered[Abstract]

= Introducción

= Definición del problema
== Problema real
== Problema técnico
=== Funcionamiento
=== Entorno
=== Vida esperada
=== Ciclo de mantenimiento
=== Competencia
=== Aspecto externo
=== Estandarización
=== Calidad y fiabilidad
=== Planificación temporal #nota[diagrama de Gantt, no lista de tareas]
=== Pruebas
=== Seguridad #nota[limitación conocida: MQTT anónimo, sin TLS]

= Objetivos #nota[ya redactado]
== Objetivo principal
== Objetivos específicos

= Antecedentes #nota[ya redactado]

= Restricciones #nota[ya redactado]
== Factores dato
== Factores estratégicos

= Estudio de alternativas tecnológicas
== Arquitectura general del sistema
== Identificación automática de cajas
=== Códigos de barras 1D
=== Códigos QR
=== RFID (LF/HF) y NFC
=== Comparativa y justificación
== Nodo embebido
=== Plataforma de microcontrolador
=== Lectores RFID candidatos
=== Lectores/módulos QR candidatos
=== Tecnologías de comunicación
=== Alimentación
== Backend y persistencia
=== Mensajería
=== Framework backend
=== Base de datos
== Frontend
=== Necesidad de visualización y alternativas de framework

= Análisis y selección de recursos
== ESP32
== RDM6300 / estándar EM4100 (RFID)
== GM861S (QR)
== Mosquitto (broker MQTT)
== FastAPI + SQLAlchemy (backend)
== Supabase / PostgreSQL (persistencia)
== React + Vite (frontend)
== Elección final

= Análisis funcional y especificación de requisitos
== Casos de uso
=== Alta de ficha de caja
=== Lectura RFID de una caja
=== Lectura QR de una caja #nota[pendiente de hardware]
=== Consulta del historial de trazabilidad
== Requisitos del sistema
=== Requisitos funcionales
=== Requisitos no funcionales
== Matriz de trazabilidad

= Diseño del sistema
== Arquitectura del sistema
== Diagrama de secuencia
== Modelo de datos #nota[fichas\_caja, historial]
== Diseño hardware
=== Lector RFID: esquema de conexión
=== Lector QR: esquema de conexión
=== Incidencias de hardware
== Diseño software
=== Firmware: captura RFID
=== Firmware: captura QR
=== Firmware: gestión de red
=== Firmware: cola local ante desconexión
=== Backend: API REST
=== Backend: suscriptor MQTT y persistencia
=== Frontend: panel de trazabilidad
== Servicios externos
=== Mosquitto (Docker)
=== Supabase

= Pruebas
== Metodología de pruebas
== Prueba 1: Captura e identificación RFID
== Prueba 2: Flujo extremo a extremo (dispositivo → base de datos)
== Prueba 3: Robustez ante desconexión (cola offline)
== Prueba 4: Captura e identificación QR #nota[pendiente]
== Prueba 5: Consulta y visualización (frontend)

= Presupuesto
== Coste de materiales
== Coste de desarrollo
== Presupuesto total

= Conclusiones

= Futuras mejoras

#unnumbered[Bibliografía #nota[ya existe]]
#unnumbered[Anexos #nota[fuera de alcance por ahora]]
#v(0.3em)
#text(size: 10pt)[
  - Manual de instalación y despliegue
  - Manual técnico / manual de usuario
]
