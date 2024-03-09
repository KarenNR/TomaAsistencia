const express = require('express');
const bodyParser = require('body-parser');
var mysql = require('mysql2');
const app = express();
const PORT = 3000;

app.use(express.json());

app.use(bodyParser.urlencoded({ extended: false }));

app.post('/', (req, res) => {
    console.log("Id recibido: " + req.body.id);
    res.status(201).json({ mensaje: 'Asistencia registrada correctamente' });
});

app.listen(PORT, () => {
    console.log(`Servidor escuchando en el puerto ${PORT}`);
});

