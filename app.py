from flask import Flask, request, render_template
import json
import threading
import time
import matplotlib.pyplot as plt
import numpy as np

app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0
mediciones_recibidas = []

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/guardarmediciones', methods=['POST'])
def guardar_mediciones():
    global mediciones_recibidas
    mediciones_json = request.form['medicionesArray']
    mediciones = json.loads(mediciones_json)
    print('Mediciones recibidas en Python:', mediciones)

    mediciones_recibidas.extend(mediciones)

    return 'Datos recibidos exitosamente'

def corregir_mediciones(mediciones):
    medicion_anterior = None
    medicion_siguiente = None

    for i, medicion in enumerate(mediciones):
        for sensor, valor in medicion.items():
            if valor == 0:
                # Buscar la medición anterior no nula
                j = i - 1
                while j >= 0:
                    if mediciones[j][sensor] != 0:
                        medicion_anterior = mediciones[j][sensor]
                        break
                    j -= 1

                # Buscar la medición siguiente no nula
                k = i + 1
                while k < len(mediciones):
                    if mediciones[k][sensor] != 0:
                        medicion_siguiente = mediciones[k][sensor]
                        break
                    k += 1

                # Calcular el promedio entre la medición anterior y siguiente
                if medicion_anterior is not None and medicion_siguiente is not None:
                    medicion_promedio = (medicion_anterior + medicion_siguiente) / 2
                    mediciones[i][sensor] = medicion_promedio
                else:
                    # Si no se pueden encontrar mediciones anteriores o siguientes, dejar el valor como está
                    mediciones[i][sensor] = valor

    return mediciones

def graficar_senoidal(mediciones):
    tiempo = np.arange(0, len(mediciones), 1)
    distancia1 = [float(medicion['distancia1']) for medicion in mediciones]
    distancia2 = [float(medicion['distancia2']) for medicion in mediciones]
    distancia3 = [float(medicion['distancia3']) for medicion in mediciones]

    plt.figure(figsize=(10, 6))
    plt.plot(tiempo, distancia1, label='Distancia 1')
    plt.plot(tiempo, distancia2, label='Distancia 2')
    plt.plot(tiempo, distancia3, label='Distancia 3')

    plt.xlabel('Tiempo (ms)')
    plt.ylabel('Distancia (cm)')
    plt.title('Mediciones en forma senoidal')
    plt.legend()
    plt.grid(True)
    
    plt.show()

def graficar_senoidal_corregida(mediciones):
    # Corregir las mediciones
    mediciones_corregidas = corregir_mediciones(mediciones)

    tiempo = np.arange(0, len(mediciones_corregidas), 1)
    distancia1 = [float(medicion['distancia1']) for medicion in mediciones_corregidas]
    distancia2 = [float(medicion['distancia2']) for medicion in mediciones_corregidas]
    distancia3 = [float(medicion['distancia3']) for medicion in mediciones_corregidas]

    plt.figure(figsize=(10, 6))
    plt.plot(tiempo, distancia1, label='Distancia 1 (Corregida)')
    plt.plot(tiempo, distancia2, label='Distancia 2 (Corregida)')
    plt.plot(tiempo, distancia3, label='Distancia 3 (Corregida)')

    plt.xlabel('Tiempo (ms)')
    plt.ylabel('Distancia (cm)')
    plt.title('Mediciones corregidas en forma senoidal')
    plt.legend()
    plt.grid(True)
    
    plt.show()

def main():
    global mediciones_recibidas
    while True:
        if mediciones_recibidas:
            print('Mediciones almacenadas:', mediciones_recibidas)
            print(len(mediciones_recibidas))
            # Graficar las mediciones originales
            graficar_senoidal(mediciones_recibidas)

            # Copiar las mediciones originales para no modificarlas
            mediciones_corregidas = list(mediciones_recibidas)

            # Graficar las mediciones corregidas
            graficar_senoidal_corregida(mediciones_corregidas)

            mediciones_recibidas = []  # Limpiar la lista después de procesar las mediciones

        time.sleep(1)  # Ajusta el retraso según sea necesario

if __name__ == '__main__':
    # Crear un hilo para ejecutar la función main() en segundo plano
    main_thread = threading.Thread(target=main)
    main_thread.start()

    # Ejecutar la aplicación Flask en el hilo principal
    app.run(debug=True, threaded=True)
