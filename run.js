const express = require('express');
const app = express();

// Network
const port = 3000;
const hostname = 'http://localhost';

// Static serve
app.use(express.static('build'));

// Disable CORS
app.use((req, res, next) => {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, DELETE');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
  next();
});

// Handle 404
app.use((req, res) => {
  res.status(404).sendFile(__dirname + '/build/404.html');
});

// Start server
app.listen(port, () => console.log(`Server running at ${hostname}:${port}`));