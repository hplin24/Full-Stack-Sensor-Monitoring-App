import { createRoot } from 'react-dom/client';
import App from './App';

const container = document.getElementById('root') as HTMLElement;
const root = createRoot(container);
root.render(<App />);

// // calling IPC exposed from preload script
// window.electron?.ipcRenderer.once('ipc-example', (arg) => {
//   // eslint-disable-next-line no-console
//   console.log(arg);
// });

// window.electron?.ipcRenderer.sendMessage('ipc-example', ['ping']);

// stm32
window.electron?.ipcRenderer.on('ipc-example', (response) => {
  // eslint-disable-next-line no-console
  const data = response as { temp: number; rh: number };
  console.log(data.temp);
  console.log(data.rh);
});
// window.electron?.ipcRenderer.sendMessage('ipc-example');

//window.electron?.ipcRenderer.sendMessage('ipc-example', 'dht11');