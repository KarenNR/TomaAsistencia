from flask import Flask, render_template, request, redirect, session, flash
from flask_mysqldb import MySQL
from datetime import datetime, timedelta

# ---------- GLOBAL VARIABLES ----------

semesterStart = datetime(2024, 1, 10)
semesterEnd = datetime(2024, 5, 10)
todayDate = datetime.now()
holidays = (datetime(2024, 2, 5), datetime(2024, 3, 18), datetime(2024, 3, 25), 
            datetime(2024, 3, 26), datetime(2024, 3, 27), datetime(2024, 3, 28), 
            datetime(2024, 3, 29), datetime(2024, 3, 30), datetime(2024, 5, 1))
classAttendance = {} # Saved for efficiency
studentAttendance = {} # Saved for efficiency

# ---------- FLASK CONFIGURATION ----------

app = Flask(__name__)
app.secret_key = "dae2024"

app.config['MYSQL_HOST'] = 'localhost'
app.config['MYSQL_USER'] = 'root'
app.config['MYSQL_PASSWORD'] = 'karengt'
app.config['MYSQL_DB'] = 'AttendanceSystem'

mysql = MySQL(app)

@app.route('/')
def loadIndex():
    return render_template("index.html")


@app.route('/login', methods=['POST'])
def login():
    _username = request.form['username']
    _password = request.form['password']
    cursor = mysql.connection.cursor()
    cursor.execute('''SELECT Nomina, Nombre, Apellido FROM Profesor WHERE Usuario=(%s) AND Pass=(%s)''', (_username, _password))
    data = cursor.fetchone()
    if data is not None:
        session['nomina'] = data[0]
        session['profesor'] = data[1] + ' ' + data[2]
        return redirect('/cursos')
    else:
        flash('El usuario o la contraseña no coinciden.')
        return redirect('/')


@app.route('/cursos')
def loadMenu():
    cursor = mysql.connection.cursor()
    cursor.execute('''SELECT * FROM Clase WHERE Nomina_Profesor=(%s)''', (session['nomina'],))
    courses = cursor.fetchall()
    # Format hour
    x = []
    for element in courses:
        y = list(element)
        y[10] = ":".join(str(element[10]).split(":")[0:2])
        y[11] = ":".join(str(element[11]).split(":")[0:2])
        element = tuple(y)
        x.append(element)
    courses = tuple(x)
    cursor.close()
    return render_template("profesor/class-menu.html", profesor=session['profesor'], cursos=courses)


@app.route('/informacion-curso/<int:id>')
def loadCourseInformation(id):
    # Get course information
    cursor = mysql.connection.cursor()
    cursor.execute('''SELECT * FROM Clase WHERE ID_Clase=(%s)''', (id,))
    courseInformation = cursor.fetchone()
    # Format hour
    x = list(courseInformation)
    x[10] = ":".join(str(courseInformation[10]).split(":")[0:2])
    x[11] = ":".join(str(courseInformation[11]).split(":")[0:2])
    courseInformation = tuple(x)
    cursor.close()
    # Get days in which class is taken
    days = courseInformation[3:10]
    # Get all students from course
    cursor = mysql.connection.cursor()
    cursor.execute('''
                    SELECT Matricula, Alumno.Nombre, Apellido, Carrera 
                    FROM Clase JOIN Alumno_Clase ON Clase.ID_Clase=Alumno_Clase.ID_Clase
                        JOIN Alumno On Matricula=Matricula_Alumno
                    WHERE Clase.ID_Clase=(%s)
                    ORDER BY Matricula''', (id,))
    students = cursor.fetchall()
    cursor.close()
    # Calculate attendance average per student
    classAverage = {"asistencia": 0, "falta": 0, "retardo": 0, "total": 0}
    studentAverages = []
    for student in students:
        attendanceInfo = getStudentAverage(id, student[0], days)
        studentAverages.append(attendanceInfo)
        classAverage["asistencia"] += attendanceInfo["asistencia"]
        classAverage["falta"] += attendanceInfo["falta"]
        classAverage["retardo"] += attendanceInfo["retardo"]
        classAverage["total"] += attendanceInfo["total"]

    return render_template("profesor/informacion-curso.html", profesor=session['profesor'], curso=courseInformation, 
                           alumnos=zip(students, studentAverages), promedioClase=classAverage, cantidadAlumnos=len(students))


@app.route('/informacion-curso/<int:courseId>/alumno/<int:studentId>')
def loadStudentInformation(courseId, studentId):
    return render_template("profesor/informacion-alumno.html", profesor=session['profesor'], curso=courseId, estudiante=studentId)


@app.route('/logout')
def logout():
    session.clear()
    return redirect('/')


# ---------- ATTENDANCE AVERAGE METHODS ----------

def getStudentAverage(courseId, studentId, days):
    startDate = semesterStart
    endDate = todayDate if todayDate < semesterEnd else semesterEnd
    results = {"asistencia": 0, "falta": 0, "retardo": 0, "total": 0}
    while startDate <= endDate:
        if days[startDate.weekday()] and startDate not in holidays:
            cursor = mysql.connection.cursor()
            cursor.execute('''SELECT Asistencia FROM Asistencia WHERE ID_Clase=(%s) AND Matricula_Alumno=(%s) AND Fecha=(%s)''',
                           (courseId, studentId, startDate.strftime("%Y-%m-%d")))
            attendance = cursor.fetchone()
            # 0 = Asistencia / 1 = Falta / 2 = Retardo
            if attendance is None:
                results["falta"] += 1
            elif attendance[0] == 0:
                results["asistencia"] += 1
            elif attendance[0] == 1:
                results["falta"] += 1
            else:
                results["retardo"] += 1
            results["total"] += 1
        startDate += timedelta(days=1)
    return results
