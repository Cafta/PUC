## 💾 Problema 001: Detecção Incompleta de Veículos Distantes

### 🔍 Problema identificado

Durante a execução do sistema de contagem de veículos com o modelo **YOLOv4-tiny**, observou-se que:

* Veículos próximos à câmera eram detectados corretamente.
* Veículos pequenos ao fundo da imagem, notadamente na região **regiao\_laranja\_E**, não eram detectados.

Isso resultava em **subcontagem** de veículos em faixas mais distantes do cruzamento.

![Imagem mostrando o erro de detecção](data\erro_de_deteccao.png)

### 🧠 Causa raiz

O modelo YOLOv4-tiny redimensiona os frames para `416x416` pixels antes da inferência. Isso afeta objetos pequenos da imagem original, pois:

* Veículos distantes já ocupam uma pequena área no frame.
* O redimensionamento reduz ainda mais essas áreas.
* O modelo não consegue identificar essas regiões como veículos com confiança suficiente.

### ✅ Solução aplicada: Detecção por zoom local (ROI)

Foi implementada uma solução baseada em **zoom virtual por região de interesse (ROI)**:

1. **Recorte da região laranja leste (regiao\_laranja\_E)** do frame.
2. **Redimensionamento do ROI** para `416x416` para simular zoom.
3. **Execução do detector YOLOv4-tiny** sobre esse recorte.
4. **Correção das coordenadas das detecções** de volta para o sistema de coordenadas do frame original.
5. **Integração da contagem** com as demais regiões processadas normalmente.

### ⚙️ Vantagens

* Permite detecção de objetos pequenos sem alterar o modelo ou resolução global.
* Preserva a leveza do YOLOv4-tiny.
* Flexível para ser usado em qualquer região do frame.

### 🔧 Possíveis melhorias futuras

* Aplicar a mesma estratégia para outras regiões críticas (ex: regiao\_azul\_E).
* Ajustar dinamicamente o `CONFIDENCE_THRESHOLD` para cada ROI.
* Avaliar modelos com maior resolução (ex: YOLOv4 full com 608x608).

---

Este documento descreve e justifica a solução técnica aplicada para melhorar a contagem de veículos em regiões distantes do campo de visão da câmera no projeto do **Semáforo Inteligente**.

---
