const express = require('express');
const bodyParser = require('body-parser');
var mysql = require('mysql2');
const app = express();
const PORT = 3000;

// Conexión a base de datos
var con = mysql.createConnection({
    /* SECRET */
});

con.connect(function (err) {
    if (err) throw err;
});

app.use(express.json());

app.use(bodyParser.urlencoded({ extended: false }));

// Registro de asistencia
app.post('/', (req, res) => {
    console.log("Id recibido: " + req.body.id);

    let matricula;
    let claseId;
    let horaInicio;

    // Obtener matrícula del estudiante
    const sql_1 = "SELECT Matricula FROM Huella_Alumno WHERE " + req.body.id + " BETWEEN LimInferior AND LimSuperior";
    con.query(sql_1, function (err, result) {
        try {
            if (err) throw err;
            if (result.length > 0) {
                matricula = result[0]['Matricula'];

                // Obtener clase actual del estudiante
                let currentDate = new Date();
                let currentDay;
                switch (currentDate.getDay()) {
                    case 0: currentDay = "D"; break;
                    case 1: currentDay = "L"; break;
                    case 2: currentDay = "M"; break;
                    case 3: currentDay = "X"; break;
                    case 4: currentDay = "J"; break;
                    case 5: currentDay = "V"; break;
                    case 6: currentDay = "S"; break;
                }
                let hours = currentDate.getHours().toString().padStart(2, '0');
                let minutes = currentDate.getMinutes().toString().padStart(2, '0');
                let seconds = currentDate.getSeconds().toString().padStart(2, '0');
                let currentHour = hours + ":" + minutes + ":" + seconds;
                const sql_2 = `
                    SELECT * 
                    FROM Clase JOIN Alumno_Clase ON Clase.ID_Clase=Alumno_Clase.ID_Clase
                    WHERE Matricula_Alumno=${matricula} 
                        AND ${currentDay}=1 AND '${currentHour}' BETWEEN DATE_SUB(Hora_Inicio, INTERVAL 10 MINUTE) AND Hora_Final
                    `;
                con.query(sql_2, function (err, result) {
                    try {
                        if (err) throw err;
                        if (result.length > 0) {
                            claseId = result[0]['ID_Clase'];
                            horaInicio = result[0]['Hora_Inicio'].split(":");

                            let inicioClase = new Date();
                            inicioClase.setHours(parseInt(horaInicio[0]), parseInt(horaInicio[1]), parseInt(horaInicio[2]));

                            // Verificar diferencia entre fecha actual e inicio de clase
                            let difference = Math.abs(Math.round((currentDate - inicioClase) / 60000));

                            // Si han pasado más de 10 minutos, poner retardo (2). En caso contrario, poner asistencia (0)
                            let code = difference > 10 ? 2 : 0;

                            // Registrar asistencia en base de datos
                            let year = currentDate.getFullYear().toString();
                            let month = (currentDate.getMonth() + 1).toString().padStart(2, '0');
                            let day = currentDate.getDate().toString().padStart(2, '0');
                            let todayDate = year + "-" + month + "-" + day;
                            const sql_3 = `INSERT INTO Asistencia VALUES (${claseId}, ${matricula}, '${todayDate}', ${code})`;

                            con.query(sql_3, function (err, result) {
                                try {
                                    if (err) throw err;
                                    res.status(201).json({ mensaje: 'Asistencia registrada correctamente' });
                                } catch (e) {
                                    res.status(201).json({ mensaje: 'La asistencia para este alumno ya se registró anteriormente' });
                                }
                            });
                        } else {
                            res.status(201).json({ mensaje: 'No hay clases actualmente' });
                        }
                    } catch (e) {
                        res.status(201).json({ mensaje: 'Error al registrar la asistencia' });
                    }
                });
            } else {
                res.status(201).json({ mensaje: 'Alumno no encontrado' });
            }
        } catch (e) {
            res.status(201).json({ mensaje: 'Error al registrar la asistencia' });
        }
    });
});

app.listen(PORT, () => {
    console.log(`Servidor escuchando en el puerto ${PORT}`);
});

