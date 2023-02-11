import React from 'react';
import { HashRouter, Route, Routes } from 'react-router-dom';
import { BaseLayout } from './components/BaseLayout/BaseLayout';
import { Dashboard } from './components/Dashboard/Dashboard';
import { Login } from './components/Login/Login';

function App() {
  return (
    <HashRouter>
        <BaseLayout>
            <Routes>
                <Route path="/" element={<Login />} />
                <Route path="/dashboard" element={<Dashboard />} />
            </Routes>
        </BaseLayout>
    </HashRouter>
  );
}

export default App;
