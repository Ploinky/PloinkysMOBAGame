import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';

window.addEventListener('DOMContentLoaded', () => {
    document.body.addEventListener('mousedown', evt => {
        if ((evt.target as any).id === '#header') {
            (window as any).chrome.webview.hostObjects.launcher.MouseDownDrag();
            evt.preventDefault();
            evt.stopPropagation();
        }
    });
});

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);

