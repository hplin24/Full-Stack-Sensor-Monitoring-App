/* eslint global-require: off, no-console: off, promise/always-return: off */

/**
 * This module executes inside of electron's main process. You can start
 * electron renderer process from here and communicate with the other processes
 * through IPC.
 *
 * When running `npm run build` or `npm run build:main`, this file is compiled to
 * `./src/main.js` using webpack. This gives us some performance wins.
 */
import path from 'path';
import { app, BrowserWindow, shell, ipcMain } from 'electron';
import { autoUpdater } from 'electron-updater';
import log from 'electron-log';
import MenuBuilder from './menu';
import { resolveHtmlPath } from './util';

import axios from 'axios';
import { ServerRes } from "../../../shared/types/ServerRes";

class AppUpdater {
  constructor() {
    log.transports.file.level = 'info';
    autoUpdater.logger = log;
    autoUpdater.checkForUpdatesAndNotify();
  }
}

let mainWindow: BrowserWindow | null = null;

// ipcMain.on('ipc-example', async (event, arg) => {
//   const msgTemplate = (pingPong: string) => `IPC test: ${pingPong}`;
//   console.log(msgTemplate(arg));
//   event.reply('ipc-example', msgTemplate('pong'));
// });

const api_server = axios.create({
  baseURL: process.env.API_BASE_URL || 'http://localhost:3000', // fallback for dev
});

ipcMain.handle('api-server', async (event, data): Promise<ServerRes> => {

  let svRes: ServerRes = {
    success: false,
    err: 0,
    message: ''
  };

  try {

    // Access the server to check whether the server addon is initializaed
    console.log('[electron:main:ipcMain] Accessing server /api/IsInit ...');
    svRes = (await api_server.get<ServerRes>('/api/IsInit')).data;

    if (!svRes.success) {
      // Server addon not yet initialized. Initialize now
      console.log('[electron:main:ipcMain] /api/IsInit reply no');
      console.log('[electron:main:ipcMain] Accessing server /api/Init ...');
      svRes = (await api_server.get('/api/Init')).data;
      if (!svRes.success) {
        console.log('[electron:main:ipcMain] /api/Init reply not ok, message: ' + svRes.message);
        return svRes;
      }
      console.log('[electron:main:ipcMain] /api/Init reply ok');
    }

    // Access the server to update the cache data (temperature and humidity) 
    console.log('[electron:main:ipcMain] Accessing server /api/UpdateData ...');
    svRes = (await api_server.get('/api/UpdateData')).data;
    if (!svRes.success) {
      console.log('[electron:main:ipcMain] /api/UpdateData reply not ok, message: ' + svRes.message);
      return svRes;
    }
    console.log('[electron:main:ipcMain] /api/UpdateData reply ok');

    // Access the server to get the cache data
    console.log('[electron:main:ipcMain] Accessing server /api/GetData ...');
    svRes = (await api_server.get('/api/GetData')).data;
    if (!svRes.success) {
      console.log('[electron:main:ipcMain] /api/GetData reply not ok, message: ' + svRes.message);
      return svRes;
    }
    console.log('[electron:main:ipcMain] /api/GetData reply ok');

  } catch (err) {
    console.error('Request error:', err);
    svRes.message = 'Request error:' + err;
    return svRes;
  }
  
  // Send the updated data back to the front-end
  return svRes;
});

if (process.env.NODE_ENV === 'production') {
  const sourceMapSupport = require('source-map-support');
  sourceMapSupport.install();
}

const isDebug =
  process.env.NODE_ENV === 'development' || process.env.DEBUG_PROD === 'true';

if (isDebug) {
  require('electron-debug').default();
}

const installExtensions = async () => {
  const installer = require('electron-devtools-installer');
  const forceDownload = !!process.env.UPGRADE_EXTENSIONS;
  const extensions = ['REACT_DEVELOPER_TOOLS'];

  return installer
    .default(
      extensions.map((name) => installer[name]),
      forceDownload,
    )
    .catch(console.log);
};

const createWindow = async () => {
  if (isDebug) {
    await installExtensions();
  }

  const RESOURCES_PATH = app.isPackaged
    ? path.join(process.resourcesPath, 'assets')
    : path.join(__dirname, '../../assets');

  const getAssetPath = (...paths: string[]): string => {
    return path.join(RESOURCES_PATH, ...paths);
  };

  mainWindow = new BrowserWindow({
    show: false,
    width: 250, // 1024
    height: 150, // 728
    icon: getAssetPath('icon.png'),
    frame: false,
    transparent: true,
    //resizable: false,
    webPreferences: {
      preload: app.isPackaged
        ? path.join(__dirname, 'preload.js')
        : path.join(__dirname, '../../.erb/dll/preload.js'),
    },
  });

  mainWindow.loadURL(resolveHtmlPath('index.html'));

  mainWindow.on('ready-to-show', () => {
    if (!mainWindow) {
      throw new Error('"mainWindow" is not defined');
    }
    if (process.env.START_MINIMIZED) {
      mainWindow.minimize();
    } else {
      mainWindow.show();
    }
  });

  mainWindow.on('closed', () => {
    mainWindow = null;
  });

  const menuBuilder = new MenuBuilder(mainWindow);
  menuBuilder.buildMenu();

  // Open urls in the user's browser
  mainWindow.webContents.setWindowOpenHandler((edata) => {
    shell.openExternal(edata.url);
    return { action: 'deny' };
  });

  // Remove this if your app does not use auto updates
  // eslint-disable-next-line
  new AppUpdater();
};

/**
 * Add event listeners...
 */

app.on('window-all-closed', () => {
  // Respect the OSX convention of having the application in memory even
  // after all windows have been closed
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app
  .whenReady()
  .then(() => {
    createWindow();
    app.on('activate', () => {
      // On macOS it's common to re-create a window in the app when the
      // dock icon is clicked and there are no other windows open.
      if (mainWindow === null) createWindow();
    });
  })
  .catch(console.log);
