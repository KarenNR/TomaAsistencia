INSERT INTO Alumno VALUES (611107, "Karen", "Garza Treviño", "ITC");
INSERT INTO Alumno VALUES (566646, "Sofía", "Montoya Chavarría", "ITC");
INSERT INTO Alumno VALUES (614571, "Fátima", "Morales Vázquez", "ITC");

INSERT INTO Huella_Alumno (Matricula, LimInferior, LimSuperior) VaLUES (611107, 5, 10);

INSERT INTO Profesor VALUES (1, "Juan", "García Montes", "juan.garcia", 12345);

INSERT INTO Clase (Nombre, Salon, L, M, X, J, V, S, D, Hora_Inicio, Hora_Final, Nomina_Profesor) VALUES ("Algoritmos I", "3204", 1, 0, 0, 1, 0, 0, 0, "10:00", "11:30", 1);
INSERT INTO Clase (Nombre, Salon, L, M, X, J, V, S, D, Hora_Inicio, Hora_Final, Nomina_Profesor) VALUES ("Algoritmos II", "3205", 0, 1, 0, 0, 1, 0, 0, "10:00", "11:30", 1);
INSERT INTO Clase (Nombre, Salon, L, M, X, J, V, S, D, Hora_Inicio, Hora_Final, Nomina_Profesor) VALUES ("Sistemas Embebidos", "3204", 1, 1, 1, 1, 1, 1, 1, "16:00", "17:30", 1);

INSERT INTO Alumno_Clase VALUES (611107, 1);
INSERT INTO Alumno_Clase VALUES (611107, 3);
INSERT INTO Alumno_Clase VALUES (566646, 1);
INSERT INTO Alumno_Clase VALUES (614571, 1);

-- 0 = Asistencia / 1 = Falta / 2 = Retardo
INSERT INTO Asistencia VALUES (1, 611107, "2024-03-07", 2);
INSERT INTO Asistencia VALUES (1, 611107, "2024-01-11", 1);