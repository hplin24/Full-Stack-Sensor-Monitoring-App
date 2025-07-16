import { MemoryRouter as Router, Routes, Route } from 'react-router-dom';
import icon from '../../assets/icon.svg';
import './App.css';
import { ApiResponse } from "../../../types/api-response";
import React, { useState } from 'react';

function Hello() {
  //
  const [data, setData] = useState<ApiResponse>({ success: false, err: 0, message: '', data: { temp: 0, rh: 0 }});

  //
  const handleClick = () => {
    window.electron?.ipcRenderer.once('ipc-example', (response) => {
      // eslint-disable-next-line no-console
      const apiRes = response as ApiResponse;
      setData({ success: apiRes.success, err: apiRes.err, message: apiRes.message, data: apiRes.data })
    });
    window.electron?.ipcRenderer.sendMessage('ipc-example');
  }
  
  //
  return (
    <div className="container">
      {/* <div className="Hello">
        <img width="200" alt="icon" src={icon} />
      </div>
      <h1>electron-react-boilerplate</h1> */}

      <div className="Hello">
        <button type="button" onClick={handleClick}>
          <span className="span-temp" role="img" aria-label="temp">
            🌡️
          </span>
          {data.data?.temp} °C<br/>
          <span className="span-rh" role="img" aria-label="rh">
            💧
          </span>
          {data.data?.rh} %
        </button>
      </div>

    </div>
  );
}

export default function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Hello />} />
      </Routes>
    </Router>
  );
}
