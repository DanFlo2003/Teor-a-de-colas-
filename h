[1mdiff --git a/app.py b/app.py[m
[1mindex 97e84c9..5670207 100644[m
[1m--- a/app.py[m
[1m+++ b/app.py[m
[36m@@ -121,7 +121,12 @@[m [mdef calculate():[m
         )[m
         [m
         # Realizar cálculos básicos[m
[31m-        results = calculator.calculate(n=n_clients)[m
[32m+[m[32m        if model == 'PICM' and n_clients is not None:[m
[32m+[m[32m            results = calculator.calculate()[m
[32m+[m[32m            for n in range(1, n_clients + 1):[m
[32m+[m[32m                results[f'P{n}'] = calculator.calcular_Pn(n)[m
[32m+[m[32m        else:[m
[32m+[m[32m            results = calculator.calculate(n=n_clients)[m
         [m
         # Calcular costos si se proporcionaron los datos[m
         cost_wait = float(data.get('cost_wait', 0))[m
[36m@@ -149,7 +154,7 @@[m [mdef calculate():[m
             'P0': 'Probabilidad de sistema vacío',[m
             'PE': 'Probabilidad de sistema ocupado',[m
             'PNE': 'Probabilidad de que no todos los servidores estén ocupados',[m
[31m-            'Pk': 'Probabilidad de k clientes en el sistema',[m
[32m+[m[32m            'Pk': 'Probabilidad de que un cliente tenga que esperar',[m
             'L': 'Número promedio de clientes en el sistema (L)',[m
             'Lq': 'Número promedio de clientes en la cola (Lq)',[m
             'Ln': 'Número esperado de clientes en cola cuando hay cola',[m
[36m@@ -163,7 +168,10 @@[m [mdef calculate():[m
             'CT': 'Costo total del sistema'[m
         }[m
         [m
[31m-        if n_clients is not None:[m
[32m+[m[32m        if model == 'PICM' and n_clients is not None:[m
[32m+[m[32m            for n in range(1, n_clients + 1):[m
[32m+[m[32m                descriptions[f'P{n}'] = f'Probabilidad de {n} clientes en el sistema'[m
[32m+[m[32m        elif n_clients is not None:[m
             descriptions[f'P{n_clients}'] = f'Probabilidad de {n_clients} clientes en el sistema'[m
         [m
         # Formatear resultados para evitar números muy largos[m
[1mdiff --git a/templates/index.html b/templates/index.html[m
[1mindex e136e1c..98e9c61 100644[m
[1m--- a/templates/index.html[m
[1m+++ b/templates/index.html[m
[36m@@ -24,88 +24,230 @@[m
         />[m
 [m
         <style>[m
[32m+[m[32m            :root {[m
[32m+[m[32m                color-scheme: light;[m
[32m+[m[32m                --ink: #172b4d;[m
[32m+[m[32m                --muted: #64748b;[m
[32m+[m[32m                --brand: #2563eb;[m
[32m+[m[32m                --brand-dark: #1d4ed8;[m
[32m+[m[32m                --line: #e2e8f0;[m
[32m+[m[32m                --surface: #ffffff;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            * {[m
[32m+[m[32m                box-sizing: border-box;[m
[32m+[m[32m            }[m
[32m+[m
             body {[m
[31m-                padding-top: 20px;[m
[31m-                background-color: #f5f5f5;[m
[32m+[m[32m                min-height: 100vh;[m
[32m+[m[32m                padding: 56px 0;[m
[32m+[m[32m                color: var(--ink);[m
[32m+[m[32m                background:[m
[32m+[m[32m                    radial-gradient([m
[32m+[m[32m                        circle at 8% 8%,[m
[32m+[m[32m                        rgba(37, 99, 235, 0.1),[m
[32m+[m[32m                        transparent 28rem[m
[32m+[m[32m                    ),[m
[32m+[m[32m                    linear-gradient(145deg, #f8fafc 0%, #eef4ff 100%);[m
[32m+[m[32m                font-family:[m
[32m+[m[32m                    Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont,[m
[32m+[m[32m                    "Segoe UI", sans-serif;[m
             }[m
 [m
             .container {[m
[31m-                max-width: 900px;[m
[32m+[m[32m                max-width: 960px;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .page-heading {[m
[32m+[m[32m                margin: 0 auto 30px;[m
[32m+[m[32m                text-align: center;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .page-eyebrow {[m
[32m+[m[32m                display: inline-flex;[m
[32m+[m[32m                align-items: center;[m
[32m+[m[32m                gap: 8px;[m
[32m+[m[32m                margin-bottom: 14px;[m
[32m+[m[32m                padding: 7px 12px;[m
[32m+[m[32m                border: 1px solid #dbeafe;[m
[32m+[m[32m                border-radius: 999px;[m
[32m+[m[32m                color: var(--brand-dark);[m
[32m+[m[32m                background: #eff6ff;[m
[32m+[m[32m                font-size: 0.75rem;[m
[32m+[m[32m                font-weight: 700;[m
[32m+[m[32m                letter-spacing: 0.08em;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .page-heading h1 {[m
[32m+[m[32m                margin-bottom: 10px;[m
[32m+[m[32m                color: var(--ink);[m
[32m+[m[32m                font-size: clamp(2rem, 5vw, 3rem);[m
[32m+[m[32m                font-weight: 750;[m
[32m+[m[32m                letter-spacing: -0.045em;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .page-heading p {[m
[32m+[m[32m                max-width: 590px;[m
[32m+[m[32m                margin: 0 auto;[m
[32m+[m[32m                color: var(--muted);[m
[32m+[m[32m                font-size: 1rem;[m
[32m+[m[32m                line-height: 1.7;[m
             }[m
 [m
             .card {[m
[31m-                border-radius: 10px;[m
[31m-                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);[m
[32m+[m[32m                overflow: hidden;[m
[32m+[m[32m                border: 1px solid rgba(203, 213, 225, 0.8);[m
[32m+[m[32m                border-radius: 18px;[m
[32m+[m[32m                background: var(--surface);[m
[32m+[m[32m                box-shadow:[m
[32m+[m[32m                    0 18px 45px rgba(30, 64, 175, 0.08),[m
[32m+[m[32m                    0 3px 10px rgba(15, 23, 42, 0.04);[m
             }[m
 [m
             .card-header {[m
[31m-                background-color: #007bff;[m
[31m-                color: white;[m
[31m-                border-radius: 10px 10px 0 0;[m
[32m+[m[32m                display: flex;[m
[32m+[m[32m                align-items: center;[m
[32m+[m[32m                gap: 12px;[m
[32m+[m[32m                padding: 20px 24px;[m
[32m+[m[32m                border: 0;[m
[32m+[m[32m                color: var(--ink);[m
[32m+[m[32m                background: var(--surface);[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .section-icon {[m
[32m+[m[32m                display: grid;[m
[32m+[m[32m                width: 42px;[m
[32m+[m[32m                height: 42px;[m
[32m+[m[32m                flex: 0 0 42px;[m
[32m+[m[32m                place-items: center;[m
[32m+[m[32m                border-radius: 13px;[m
[32m+[m[32m                color: var(--brand);[m
[32m+[m[32m                background: #eff6ff;[m
[32m+[m[32m                font-size: 1.15rem;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .card-header h4 {[m
[32m+[m[32m                font-size: 1.1rem;[m
[32m+[m[32m                font-weight: 700;[m
[32m+[m[32m                letter-spacing: -0.02em;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .card-body {[m
[32m+[m[32m                padding: 8px 24px 26px;[m
             }[m
 [m
             .btn-primary {[m
[31m-                background-color: #007bff;[m
[31m-                border-color: #007bff;[m
[32m+[m[32m                padding: 11px 20px;[m
[32m+[m[32m                border-color: var(--brand);[m
[32m+[m[32m                border-radius: 10px;[m
[32m+[m[32m                background-color: var(--brand);[m
[32m+[m[32m                box-shadow: 0 5px 12px rgba(37, 99, 235, 0.2);[m
[32m+[m[32m                font-weight: 650;[m
[32m+[m[32m                transition:[m
[32m+[m[32m                    background-color 0.2s ease,[m
[32m+[m[32m                    box-shadow 0.2s ease,[m
[32m+[m[32m                    transform 0.2s ease;[m
             }[m
 [m
             .btn-primary:hover {[m
[31m-                background-color: #0069d9;[m
[31m-                border-color: #0062cc;[m
[32m+[m[32m                transform: translateY(-1px);[m
[32m+[m[32m                border-color: var(--brand-dark);[m
[32m+[m[32m                background-color: var(--brand-dark);[m
[32m+[m[32m                box-shadow: 0 8px 18px rgba(37, 99, 235, 0.25);[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .btn-primary:disabled {[m
[32m+[m[32m                box-shadow: none;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .btn-outline-secondary {[m
[32m+[m[32m                padding: 9px 13px;[m
[32m+[m[32m                border-color: #cbd5e1;[m
[32m+[m[32m                border-radius: 9px;[m
[32m+[m[32m                color: #475569;[m
[32m+[m[32m                font-weight: 600;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .form-label {[m
[32m+[m[32m                margin-bottom: 8px;[m
[32m+[m[32m                color: #334155;[m
[32m+[m[32m                font-size: 0.92rem;[m
[32m+[m[32m                font-weight: 600;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .form-control,[m
[32m+[m[32m            .form-select {[m
[32m+[m[32m                min-height: 46px;[m
[32m+[m[32m                border-color: #d7e0ec;[m
[32m+[m[32m                border-radius: 10px;[m
[32m+[m[32m                color: var(--ink);[m
[32m+[m[32m                background-color: #fff;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            .form-control:focus,[m
[32m+[m[32m            .form-select:focus {[m
[32m+[m[32m                border-color: #7aa7fa;[m
[32m+[m[32m                box-shadow: 0 0 0 4px rgba(37, 99, 235, 0.12);[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            #model {[m
[32m+[m[32m                max-width: 100%;[m
             }[m
 [m
             .result-row {[m
[31m-                border-bottom: 1px solid #eee;[m
[31m-                padding: 8px 0;[m
[32m+[m[32m                padding: 12px 0;[m
[32m+[m[32m                border-bottom: 1px solid var(--line);[m
             }[m
 [m
             .result-value {[m
[31m-                font-weight: bold;[m
[31m-                color: #007bff;[m
[32m+[m[32m                color: var(--brand-dark);[m
[32m+[m[32m                font-weight: 700;[m
             }[m
 [m
             .loader {[m
[31m-                border: 4px solid #f3f3f3;[m
[31m-                border-top: 4px solid #007bff;[m
[32m+[m[32m                width: 38px;[m
[32m+[m[32m                height: 38px;[m
[32m+[m[32m                margin: 22px auto;[m
[32m+[m[32m                border: 4px solid #dbeafe;[m
[32m+[m[32m                border-top-color: var(--brand);[m
                 border-radius: 50%;[m
[31m-                width: 40px;[m
[31m-                height: 40px;[m
[31m-                animation: spin 2s linear infinite;[m
[31m-                margin: 20px auto;[m
                 display: none;[m
[32m+[m[32m                animation: spin 0.8s linear infinite;[m
             }[m
 [m
             @keyframes spin {[m
[31m-                0% {[m
[31m-                    transform: rotate(0deg);[m
[31m-                }[m
[31m-[m
[31m-                100% {[m
[32m+[m[32m                to {[m
                     transform: rotate(360deg);[m
                 }[m
             }[m
 [m
             .model-desc {[m
[31m-                margin-top: 10px;[m
[31m-                font-style: italic;[m
[31m-                color: #666;[m
[32m+[m[32m                min-height: 24px;[m
[32m+[m[32m                margin-top: 9px;[m
[32m+[m[32m                color: var(--muted);[m
[32m+[m[32m                font-size: 0.88rem;[m
             }[m
 [m
             .model-desc i:hover {[m
[32m+[m[32m                color: var(--brand);[m
                 opacity: 1 !important;[m
[31m-                color: #007bff;[m
[31m-                transition: all 0.2s ease;[m
[32m+[m[32m                transition: color 0.2s ease;[m
             }[m
 [m
             .modal-content {[m
[31m-                border-radius: 10px;[m
[32m+[m[32m                overflow: hidden;[m
[32m+[m[32m                border: 0;[m
[32m+[m[32m                border-radius: 16px;[m
[32m+[m[32m                box-shadow: 0 24px 70px rgba(15, 23, 42, 0.2);[m
             }[m
 [m
             .modal-header {[m
[31m-                border-bottom: none;[m
[32m+[m[32m                padding: 18px 22px;[m
[32m+[m[32m                border-bottom: 1px solid var(--line);[m
             }[m
 [m
             .modal-body {[m
[32m+[m[32m                padding: 22px;[m
                 text-align: center;[m
             }[m
 [m
[36m@@ -114,19 +256,54 @@[m
                 height: auto;[m
                 display: block;[m
                 margin: 0 auto;[m
[31m-                /* El cursor lo manejará Fancybox, pero lo dejamos si quieres un hint visual */[m
                 cursor: zoom-in;[m
             }[m
[32m+[m
[32m+[m[32m            .alert-danger {[m
[32m+[m[32m                border: 1px solid #fecaca;[m
[32m+[m[32m                border-radius: 12px;[m
[32m+[m[32m            }[m
[32m+[m
[32m+[m[32m            @media (max-width: 575.98px) {[m
[32m+[m[32m                body {[m
[32m+[m[32m                    padding: 34px 0;[m
[32m+[m[32m                }[m
[32m+[m
[32m+[m[32m                .page-heading {[m
[32m+[m[32m                    margin-bottom: 22px;[m
[32m+[m[32m                }[m
[32m+[m
[32m+[m[32m                .card-header {[m
[32m+[m[32m                    padding: 17px 18px;[m
[32m+[m[32m                }[m
[32m+[m
[32m+[m[32m                .card-body {[m
[32m+[m[32m                    padding: 6px 18px 21px;[m
[32m+[m[32m                }[m
[32m+[m[32m            }[m
         </style>[m
     </head>[m
 [m
     <body>[m
         <div class="container">[m
[31m-            <h1 class="text-center mb-4">Calculadora de Teoría de Colas</h1>[m
[32m+[m[32m            <header class="page-heading">[m
[32m+[m[32m                <div class="page-eyebrow">[m
[32m+[m[32m                    <i class="bi bi-graph-up-arrow" aria-hidden="true"></i>[m
[32m+[m[32m                    ANÁLISIS DE SISTEMAS DE ESPERA[m
[32m+[m[32m                </div>[m
[32m+[m[32m                <h1>Calculadora de Teoría de Colas</h1>[m
[32m+[m[32m                <p>[m
[32m+[m[32m                    Selecciona un modelo, ingresa sus parámetros y consulta los[m
[32m+[m[32m                    indicadores de desempeño de tu sistema.[m
[32m+[m[32m                </p>[m
[32m+[m[32m            </header>[m
 [m
             <div class="card mb-4">[m
                 <div class="card-header">[m
[31m-                    <h4 class="mb-0">Parámetros del modelo</h4>[m
[32m+[m[32m                    <span class="section-icon" aria-hidden="true">[m
[32m+[m[32m                        <i class="bi bi-sliders"></i>[m
[32m+[m[32m                    </span>[m
[32m+[m[32m                    <h4 class="mb-0">Configura tu modelo</h4>[m
                 </div>[m
                 <div class="card-body">[m
                     <div class="row">[m
[36m@@ -195,7 +372,10 @@[m
 [m
             <div id="results-container" class="card mb-4" style="display: none">[m
                 <div class="card-header">[m
[31m-                    <h4 class="mb-0">Resultados</h4>[m
[32m+[m[32m                    <span class="section-icon" aria-hidden="true">[m
[32m+[m[32m                        <i class="bi bi-bar-chart-line"></i>[m
[32m+[m[32m                    </span>[m
[32m+[m[32m                    <h4 class="mb-0">Resultados del análisis</h4>[m
                 </div>[m
                 <div class="card-body">[m
                     <div id="results"></div>[m
[36m@@ -535,14 +715,25 @@[m
                     const tbody = document.createElement("tbody");[m
 [m
                     // Orden específico de los parámetros[m
[32m+[m[32m                    const probabilityParams = Object.keys(results)[m
[32m+[m[32m                        .filter((key) => /^P\d+$/.test(key) && key !== "P0")[m
[32m+[m[32m                        .sort([m
[32m+[m[32m                            (a, b) =>[m
[32m+[m[32m                                Number(a.slice(1)) - Number(b.slice(1))[m
[32m+[m[32m                        );[m
                     const orderedParams = [[m
                         "ρ",[m
                         "P0",[m
[31m-                        "Pn",[m
[32m+[m[32m                        ...probabilityParams,[m
[32m+[m[32m                        "Pk",[m
[32m+[m[32m                        "PE",[m
[32m+[m[32m                        "PNE",[m
                         "Lq",[m
                         "L",[m
[32m+[m[32m                        "Ln",[m
                         "Wq",[m
                         "W",[m
[32m+[m[32m                        "Wn",[m
                     ];[m
 [m
                     // Función para crear el botón de copia[m
