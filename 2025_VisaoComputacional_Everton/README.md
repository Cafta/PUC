# Wrist Hunter Game - Estrutura com Design Patterns

## 📁 Estrutura do Projeto

```
wrist-hunter-game/
├── main.py              # Ponto de entrada principal
├── game_manager.py      # Gerenciador principal (Singleton Pattern)
├── game_objects.py      # Classes dos objetos do jogo
├── phase_system.py      # Sistema de fases (Strategy Pattern)
├── event_system.py      # Sistema de eventos (Observer Pattern)
└── README.md           # Documentação
```

## 🎮 Visão Geral do Jogo

**Wrist Hunter** é um jogo de realidade aumentada que usa MediaPipe para detectar a posição dos punhos do jogador. O objetivo é:
- ✅ Colocar os punhos nos círculos **AZUIS** para ganhar pontos
- ❌ Evitar círculos **VERMELHOS** que causam dano
- 🔥 Construir combos para multiplicadores de pontos

## 🏗️ Design Patterns Implementados

### 1. **Singleton Pattern** (`GameManager`)
- Garante uma única instância do gerenciador do jogo
- Centraliza o controle de todos os sistemas
- Facilita acesso global ao estado do jogo

### 2. **Strategy Pattern** (`PhaseStrategy`)
- Permite diferentes comportamentos de fase
- Fases implementadas:
  - `StaticCirclesPhase`: Círculos estáticos (Fase 1)
  - `MovingCirclesPhase`: Círculos em movimento (Fase 2)
  - `WavePhase`: Ondas com padrões específicos (Fase 3)
- Facilita adição de novas fases sem modificar código existente

### 3. **Observer Pattern** (`EventManager`)
- Sistema de eventos desacoplado
- Eventos suportados:
  - `COLLISION_BLUE`: Colisão com círculo azul
  - `COLLISION_RED`: Colisão com círculo vermelho
  - `PHASE_COMPLETE`: Fase completada
  - `GAME_OVER`: Fim de jogo
  - `SCORE_MILESTONE`: Marco de pontuação
  - `COMBO_ACHIEVED`: Combo alcançado
- Permite que múltiplos sistemas reajam a eventos

### 4. **Factory Pattern** (implícito)
- Criação de círculos através das estratégias de fase
- Diferentes tipos de círculos (`Circle`, `MovingCircle`)

### 5. **State Pattern** (`GameState`)
- Estados do jogo:
  - `MENU`: Tela inicial
  - `PLAYING`: Jogando
  - `PAUSED`: Pausado
  - `GAME_OVER`: Fim de jogo
  - `PHASE_TRANSITION`: Transição entre fases

## 📦 Classes Principais

### `GameObject` (Abstract Base Class)
```python
class GameObject(ABC):
    def update(delta_time: float)  # Atualiza estado
    def render(frame: np.ndarray)  # Renderiza objeto
```

### `Circle`
- Representa um círculo no jogo
- Tipos: `BLUE` (bom) ou `RED` (ruim)
- Detecta colisões com punhos
- Suporta transparência e efeitos visuais

### `Wrist`
- Representa um punho detectado
- Mantém histórico de posições (trilha visual)
- Lados: esquerdo ou direito

### `EventManager`
- Gerencia inscrições e emissões de eventos
- Mantém histórico de eventos
- Desacopla comunicação entre sistemas

### `ScoreManager`
- Gerencia pontuação, vidas e combos
- Sistema de multiplicadores
- Marcos de pontuação (achievements)

### `EffectsManager`
- Gerencia efeitos visuais
- Tipos de efeitos:
  - Explosão de partículas
  - Shake de tela
  - Flash
  - Texto de combo
  - Conquistas

## 🎯 Sistema de Gameplay

### Pontuação
- Círculos azuis: +10 pontos base
- Sistema de combo aumenta multiplicador:
  - 5+ combo: x2 multiplicador
  - 10+ combo: x3 multiplicador
- Círculos vermelhos resetam combo

### Vidas
- Começa com 3 vidas
- Círculos vermelhos causam -1 vida
- Ganha +1 vida ao completar fase
- Máximo de 5 vidas

### Fases
- Cada fase dura 30 segundos
- Dificuldade progressiva
- Após fase 3, reinicia com dificuldade aumentada

## 🚀 Como Executar

### Requisitos
```bash
pip install opencv-python mediapipe numpy
```

### Execução
```bash
python main.py
```

### Controles
- **ESPAÇO**: Iniciar/Pausar
- **D**: Modo debug
- **E**: Toggle efeitos visuais
- **Q**: Sair

## 🔧 Extensibilidade

### Adicionar Nova Fase
```python
class CustomPhase(PhaseStrategy):
    def initialize(self):
        # Configuração inicial
        
    def update(self, delta_time):
        # Lógica de atualização
        
    def spawn_circles(self):
        # Lógica de spawn customizada
```

### Adicionar Novo Tipo de Círculo
```python
class PowerUpCircle(Circle):
    def __init__(self, position, radius):
        super().__init__(position, radius, CircleType.BLUE)
        self.power_type = "extra_life"
        
    def update(self, delta_time):
        # Comportamento especial
```

### Adicionar Novo Evento
```python
# Em EventType
POWER_UP_COLLECTED = "power_up_collected"

# Emitir evento
event_manager.emit(GameEvent(
    EventType.POWER_UP_COLLECTED,
    {'type': 'extra_life'},
    timestamp
))
```

## 🎨 Melhorias Futuras

1. **Sistema de Power-ups**
   - Escudo temporário
   - Slow motion
   - Pontos duplos

2. **Modo Multiplayer**
   - Competitivo local
   - Cooperativo

3. **Sistema de Progressão**
   - Desbloqueio de fases
   - Customização visual
   - Leaderboard

4. **Efeitos Sonoros**
   - Música de fundo
   - Sons de colisão
   - Feedback auditivo

5. **Análise de Performance**
   - Heatmap de movimentos
   - Estatísticas detalhadas
   - Replay de melhores jogadas

## 📝 Notas Técnicas

- **FPS**: Otimizado para 30-60 FPS
- **Resolução**: 1280x720 (ajustável)
- **Detecção**: MediaPipe Pose com confiança 0.5
- **Colisão**: Detecção por distância euclidiana
- **Renderização**: OpenCV com transparência alpha

## 🤝 Contribuindo

Para adicionar novas funcionalidades:
1. Siga os padrões estabelecidos
2. Use os design patterns apropriados
3. Adicione eventos para comunicação entre sistemas
4. Mantenha o código desacoplado e modular

## 📄 Licença

Este projeto é de código aberto para fins educacionais.
