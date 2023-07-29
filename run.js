const express = require('express');
const app = express();

// Network
const port = 3000;
const hostname = 'http://localhost';

// Static serve
app.use(express.static('src'));

// Start server
app.listen(port, () => console.log(`Server running at ${hostname}:${port}`));