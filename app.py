from flask import Flask, render_template, request, jsonify
from teoria_de_colas import QueueTheoryCalculator  # Asumo que guardaste la clase en este módulo

app = Flask(__name__)

TIME_UNIT_TO_HOURS = {
    'segundos': 1 / 3600,
    'minutos': 1 / 60,
    'horas': 1
}

TIME_UNIT_LABELS = {
    'segundos': 'segundos',
    'minutos': 'minutos',
    'horas': 'horas'
}

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/get_params', methods=['POST'])
def get_params():
    data = request.get_json()
    model = data.get('model')
    print(f"Modelo recibido: {model}")
    
    # Definir los parámetros necesarios para cada modelo
    params_config = {
        'PICS': {  # M/M/1
            'required': ['lambda', 'mu'],
            'optional': ['n_clients', 'cost_wait', 'cost_system', 'cost_service', 'cost_server', 'hours'],
            'descriptions': {
                'lambda': 'Tasa de llegada (λ)',
                'mu': 'Tasa de servicio (μ)',
                'n_clients': 'Número de clientes para calcular P(n)',
                'cost_wait': 'Costo unitario por tiempo de espera',
                'cost_system': 'Costo unitario por tiempo en el sistema',
                'cost_service': 'Costo unitario por tiempo de servicio',
                'cost_server': 'Costo diario por servidor',
                'hours': 'Horas laborables'
            }
        },
        'PICM': {  # M/M/k
            'required': ['lambda', 'mu', 'k'],
            'optional': ['n_clients', 'cost_wait', 'cost_system', 'cost_service', 'cost_server', 'hours'],
            'descriptions': {
                'lambda': 'Tasa de llegada (λ)',
                'mu': 'Tasa de servicio (μ)',
                'k': 'Número de servidores',
                'n_clients': 'Número de clientes para calcular P(n)',
                'cost_wait': 'Costo unitario por tiempo de espera',
                'cost_system': 'Costo unitario por tiempo en el sistema',
                'cost_service': 'Costo unitario por tiempo de servicio',
                'cost_server': 'Costo diario por servidor',
                'hours': 'Horas laborables'
            }
        },
        'PFCS': {  # M/M/1/M/M
            'required': ['lambda', 'mu', 'M'],
            'optional': ['n_clients', 'cost_wait', 'cost_system', 'cost_service', 'cost_server', 'hours'],
            'descriptions': {
                'lambda': 'Tasa de llegada (λ)',
                'mu': 'Tasa de servicio (μ)',
                'M': 'Tamaño de la población',
                'n_clients': 'Número de clientes para calcular P(n)',
                'cost_wait': 'Costo unitario por tiempo de espera',
                'cost_system': 'Costo unitario por tiempo en el sistema',
                'cost_service': 'Costo unitario por tiempo de servicio',
                'cost_server': 'Costo diario por servidor',
                'hours': 'Horas laborables'
            }
        },
        'PFCM': {  # M/M/k/M/M
            'required': ['lambda', 'mu', 'k', 'M'],
            'optional': ['n_clients', 'cost_wait', 'cost_system', 'cost_service', 'cost_server', 'hours'],
            'descriptions': {
                'lambda': 'Tasa de llegada (λ)',
                'mu': 'Tasa de servicio (μ)',
                'k': 'Número de servidores',
                'M': 'Tamaño de la población',
                'n_clients': 'Número de clientes para calcular P(n)',
                'cost_wait': 'Costo unitario por tiempo de espera',
                'cost_system': 'Costo unitario por tiempo en el sistema',
                'cost_service': 'Costo unitario por tiempo de servicio',
                'cost_server': 'Costo diario por servidor',
                'hours': 'Horas laborables'
            }
        }
    }
    
    if model not in params_config:
        return jsonify({'error': 'Modelo no válido'})
    
    config = params_config[model]
    return jsonify({
        'required_params': config['required'],
        'descriptions': {k: v for k, v in config['descriptions'].items() if k in config['required']},
        'optional_params': config['optional'],
        'optional_descriptions': {k: v for k, v in config['descriptions'].items() if k in config['optional']}
    })


@app.route('/calculate', methods=['POST'])
def calculate():
    try:
        
        data = request.get_json()
        model = data.get('model')
        print(f"Modelo recibido para cálculo: {model}")
        
        if not model:
            return jsonify({'error': 'No se ha seleccionado ningún modelo'})
        
        # Mapeo de modelos a opciones numéricas
        model_mapping = {
            'PICS': 1,
            'PICM': 2,
            'PFCS': 3,
            'PFCM': 4
        }
        
        # Crear instancia del calculador
        calculator = QueueTheoryCalculator()
        
        # Configurar parámetros básicos
        
        time_unit = data.get('time_unit', 'horas')
        if time_unit not in TIME_UNIT_TO_HOURS:
            return jsonify({'error': 'Unidad de tiempo no válida'}), 400

        # Las fórmulas y los costos trabajan internamente con horas.
        rate_to_hour = 1 / TIME_UNIT_TO_HOURS[time_unit]
        lambda_val = float(data.get('lambda', 0)) * rate_to_hour
        mu = float(data.get('mu', 0)) * rate_to_hour
        k = int(data.get('k', 1))
        M = int(data.get('M', 0)) if data.get('M') else None
        n_clients = int(data.get('n_clients')) if data.get('n_clients') else None
        
        # Configurar el modelo y parámetros
        calculator.set_parameters(
            lambda_=lambda_val, 
            mu=mu, 
            k=k, 
            M=M, 
            op=model_mapping[model]
        )
        
        # Realizar cálculos básicos
        if model == 'PICM' and n_clients is not None:
            results = calculator.calculate()
            for n in range(1, n_clients + 1):
                results[f'P{n}'] = calculator.calcular_Pn(n)
        else:
            results = calculator.calculate(n=n_clients)
        
        # Calcular costos si se proporcionaron los datos
        cost_wait = float(data.get('cost_wait', 0))
        cost_system = float(data.get('cost_system', cost_wait))
        cost_service = float(data.get('cost_service', cost_wait))
        cost_server = float(data.get('cost_server', 0))
        hours = float(data.get('hours', 8))
        
        if any(cost > 0 for cost in (cost_wait, cost_system, cost_service, cost_server)):
            calculator.set_cost_parameters(
                costo_espera=cost_wait,
                costo_sistema=cost_system,
                costo_servicio=cost_service,
                costo_servidor=cost_server,
                horas_laborables=hours
            )
            costs = {
                'CTte': calculator.calcular_CTte(),
                'CTts': calculator.calcular_CTts(),
                'CTse': calculator.calcular_CTse(),
                'CTs': calculator.calcular_CTs(),
                'CT': calculator.calcular_CT()
            }
            results.update(costs)

        # Convertir los tiempos internos (horas) a la unidad elegida.
        time_result_factor = 1 / TIME_UNIT_TO_HOURS[time_unit]
        for key in ('W', 'Wq', 'Wn'):
            if key in results:
                results[key] *= time_result_factor

        # Preparar descripciones para los resultados
        time_unit_label = TIME_UNIT_LABELS[time_unit]
        descriptions = {
            'ρ': 'Factor de utilización (ρ)',
            'P0': 'Probabilidad de sistema vacío',
            'PE': 'Probabilidad de sistema ocupado',
            'PNE': 'Probabilidad de que no todos los servidores estén ocupados',
            'Pk': 'Probabilidad de que un cliente tenga que esperar',
            'L': 'Número promedio de clientes en el sistema (L)',
            'Lq': 'Número promedio de clientes en la cola (Lq)',
            'Ln': 'Número esperado de clientes en cola cuando hay cola',
            'W': f'Tiempo promedio en el sistema (W), en {time_unit_label}',
            'Wq': f'Tiempo promedio en la cola (Wq), en {time_unit_label}',
            'Wn': f'Tiempo de espera en cola cuando hay cola, en {time_unit_label}',
            'CTte': 'Costo total del tiempo de espera',
            'CTts': 'Costo total del tiempo en el sistema',
            'CTse': 'Costo total del tiempo de servicio',
            'CTs': 'Costo total de los servidores',
            'CT': 'Costo total del sistema'
        }
        
        if model == 'PICM' and n_clients is not None:
            for n in range(1, n_clients + 1):
                descriptions[f'P{n}'] = f'Probabilidad de {n} clientes en el sistema'
        elif n_clients is not None:
            descriptions[f'P{n_clients}'] = f'Probabilidad de {n_clients} clientes en el sistema'
        
        # Formatear resultados para evitar números muy largos
        formatted_results = {}
        for key, value in results.items():
            # Redondear a 6 decimales y eliminar ceros innecesarios
            formatted_value = round(value, 6)
            if formatted_value == int(formatted_value):
                formatted_value = int(formatted_value)
            formatted_results[key] = formatted_value
        print(f"Resultados calculados: {formatted_results}")
        return jsonify({
            'results': formatted_results,
            'descriptions': descriptions
        })
    
    except ValueError as ve:
        return jsonify({'error': f"Error en los valores proporcionados: {str(ve)}"})
    except Exception as e:
        return jsonify({'error': f"Error inesperado: {str(e)}"})

if __name__ == '__main__':
    app.run(debug=True)