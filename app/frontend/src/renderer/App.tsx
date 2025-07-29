import { MemoryRouter as Router, Routes, Route } from 'react-router-dom';
import icon from '../../assets/icon.svg';
import './App.css';
import { ServerRes } from "../../../shared/types/ServerRes";
import React, { useState, useEffect } from 'react';

function Home() {
  const [data, setData] = useState<ServerRes>({
    success: false, err: 0, message: '', data: { temp: -1, humidity: -1 }
  });

  const FetchSensorData = async () => {
    const svRes = await window.electron?.ipcRenderer.invokeMessage('api-server');
    setData(svRes);
  };

  useEffect(() => {
    FetchSensorData();
    // Fetch sensor data every 10 seconds
    const intervalId = setInterval(() => { FetchSensorData(); }, 10000); 
    // Cleanup when unmount
    return () => clearInterval(intervalId);
  }, []);

  const fetchDataNow = () => {
     FetchSensorData();
  }

  return (
    <div className="container">
      {
      /* <div className="Home">
        <img width="200" alt="icon" src={icon} />
      </div>
      <h1>electron-react-boilerplate</h1> */}

      <div className="Home">
        <button type="button" onClick={fetchDataNow}>
          <span className="span-temp" role="img" aria-label="temp">
            🌡️
          </span>
          {data.data?.temp} °C<br/>
          <span className="span-rh" role="img" aria-label="rh">
            💧
          </span>
          {data.data?.humidity} %
        </button>
      </div>
    </div>
  );
}

export default function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<Home />} />
      </Routes>
    </Router>
  );
}
