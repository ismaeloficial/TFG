import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    // Puerto fijo (5173 lo usa a veces Venatus-2) para no tener que
    // reajustar CORS en el backend cada vez que arranca en un puerto distinto.
    port: 5175,
    strictPort: true,
  },
})
