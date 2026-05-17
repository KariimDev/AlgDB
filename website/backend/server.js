const express = require('express');
const cors = require('cors');
const multer = require('multer');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');
const os = require('os');

const app = express();
app.use(cors());
app.use(express.json());

const BRIDGE_EXE = path.join(__dirname, '../../web_bridge.exe');

// Store uploaded file path in memory for current session
let currentDbPath = null;

// Multer: save uploaded file to a temp location
const upload = multer({
    dest: os.tmpdir(),
    fileFilter: (req, file, cb) => {
        cb(null, true);
    }
});

function runBridge(cmd, dbPath, res) {
    execFile(BRIDGE_EXE, [cmd, dbPath], { maxBuffer: 1024 * 1024 * 10 }, (error, stdout) => {
        if (error) {
            console.error(error);
            return res.status(500).json({ error: 'Bridge execution failed', detail: error.message });
        }
        try {
            const data = JSON.parse(stdout);
            res.json(data);
        } catch (e) {
            res.status(500).json({ error: 'Invalid JSON from bridge', raw: stdout.substring(0, 300) });
        }
    });
}

// Upload endpoint — accepts a .txt file and stores the path
app.post('/api/upload', upload.single('db'), (req, res) => {
    if (!req.file) return res.status(400).json({ error: 'No file uploaded' });
    currentDbPath = req.file.path;
    res.json({ ok: true, filename: req.file.originalname });
});

app.get('/api/personalities', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    runBridge('get_personalities', currentDbPath, res);
});

app.get('/api/events', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    runBridge('get_events', currentDbPath, res);
});

app.get('/api/bst', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    runBridge('get_bst', currentDbPath, res);
});

app.post('/api/personality', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    const { name, dob, dod, definition } = req.body;
    if (!name) return res.status(400).json({ error: 'Name is required' });
    
    execFile(BRIDGE_EXE, ['add_personality', currentDbPath, name, dob || '', dod || '', definition || ''], (error, stdout) => {
        if (error) return res.status(500).json({ error: 'Failed to add' });
        try { res.json(JSON.parse(stdout)); } catch (e) { res.status(500).json({ error: 'Parse error' }); }
    });
});

app.delete('/api/personality/:name', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    const name = req.params.name;
    execFile(BRIDGE_EXE, ['delete_personality', currentDbPath, name], (error, stdout) => {
        if (error) return res.status(500).json({ error: 'Failed to delete personality' });
        try { res.json(JSON.parse(stdout)); } catch (e) { res.status(500).json({ error: 'Parse error' }); }
    });
});

app.delete('/api/event/:name', (req, res) => {
    if (!currentDbPath) return res.status(400).json({ error: 'No file loaded' });
    const name = req.params.name;
    execFile(BRIDGE_EXE, ['delete_event', currentDbPath, name], (error, stdout) => {
        if (error) return res.status(500).json({ error: 'Failed to delete event' });
        try { res.json(JSON.parse(stdout)); } catch (e) { res.status(500).json({ error: 'Parse error' }); }
    });
});

const PORT = 3000;
app.listen(PORT, () => console.log(`AlgDB backend running on http://localhost:${PORT}`));
