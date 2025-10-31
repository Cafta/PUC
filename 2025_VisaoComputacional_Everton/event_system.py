"""
Sistema de eventos usando Observer Pattern
"""
from typing import List, Dict, Any, Callable
from dataclasses import dataclass
from enum import Enum


class EventType(Enum):
    """Tipos de eventos do jogo"""
    COLLISION_BLUE = "collision_blue"
    COLLISION_RED = "collision_red"
    PHASE_COMPLETE = "phase_complete"
    GAME_OVER = "game_over"
    SCORE_MILESTONE = "score_milestone"
    LIFE_LOST = "life_lost"
    COMBO_ACHIEVED = "combo_achieved"


@dataclass
class GameEvent:
    """Classe representando um evento do jogo"""
    event_type: EventType
    data: Dict[str, Any]
    timestamp: float


class EventManager:
    """Gerenciador de eventos usando Observer Pattern"""
    
    def __init__(self):
        self._observers: Dict[EventType, List[Callable]] = {}
        self._event_history: List[GameEvent] = []
        
    def subscribe(self, event_type: EventType, callback: Callable):
        """Inscreve um observador para um tipo de evento"""
        if event_type not in self._observers:
            self._observers[event_type] = []
        self._observers[event_type].append(callback)
        
    def unsubscribe(self, event_type: EventType, callback: Callable):
        """Remove um observador"""
        if event_type in self._observers:
            self._observers[event_type].remove(callback)
            
    def emit(self, event: GameEvent):
        """Emite um evento para todos os observadores inscritos"""
        self._event_history.append(event)
        
        if event.event_type in self._observers:
            for callback in self._observers[event.event_type]:
                callback(event)
                
    def get_history(self, event_type: EventType = None) -> List[GameEvent]:
        """Retorna histórico de eventos"""
        if event_type:
            return [e for e in self._event_history if e.event_type == event_type]
        return self._event_history.copy()


class ScoreManager:
    """Gerenciador de pontuação e vidas"""
    
    def __init__(self, event_manager: EventManager):
        self.event_manager = event_manager
        self.score = 0
        self.lives = 3
        self.max_lives = 5
        self.combo = 0
        self.max_combo = 0
        self.multiplier = 1
        
        # Marcos de pontuação para achievements
        self.score_milestones = [100, 500, 1000, 2500, 5000, 10000]
        self.reached_milestones = set()
        
        # Inscreve-se aos eventos relevantes
        event_manager.subscribe(EventType.COLLISION_BLUE, self._on_blue_collision)
        event_manager.subscribe(EventType.COLLISION_RED, self._on_red_collision)
        
    def _on_blue_collision(self, event: GameEvent):
        """Processa colisão com círculo azul"""
        points = event.data.get('points', 10)
        self.add_score(points)
        self.combo += 1
        
        # Atualiza multiplicador baseado no combo
        if self.combo >= 10:
            self.multiplier = 3
        elif self.combo >= 5:
            self.multiplier = 2
        else:
            self.multiplier = 1
            
        # Verifica se atingiu novo recorde de combo
        if self.combo > self.max_combo:
            self.max_combo = self.combo
            if self.combo % 10 == 0:  # A cada 10 de combo
                self.event_manager.emit(GameEvent(
                    EventType.COMBO_ACHIEVED,
                    {'combo': self.combo, 'multiplier': self.multiplier},
                    event.timestamp
                ))
                
    def _on_red_collision(self, event: GameEvent):
        """Processa colisão com círculo vermelho"""
        damage = event.data.get('damage', 1)
        self.lose_life(damage)
        self.combo = 0  # Reset combo
        self.multiplier = 1
        
    def add_score(self, points: int):
        """Adiciona pontos com multiplicador"""
        actual_points = points * self.multiplier
        self.score += actual_points
        
        # Verifica marcos de pontuação
        for milestone in self.score_milestones:
            if self.score >= milestone and milestone not in self.reached_milestones:
                self.reached_milestones.add(milestone)
                self.event_manager.emit(GameEvent(
                    EventType.SCORE_MILESTONE,
                    {'milestone': milestone, 'total_score': self.score},
                    0  # Timestamp será definido pelo jogo
                ))
                
    def lose_life(self, amount: int = 1):
        """Perde vidas"""
        self.lives = max(0, self.lives - amount)
        
        self.event_manager.emit(GameEvent(
            EventType.LIFE_LOST,
            {'remaining_lives': self.lives},
            0
        ))
        
        if self.lives <= 0:
            self.event_manager.emit(GameEvent(
                EventType.GAME_OVER,
                {'final_score': self.score, 'max_combo': self.max_combo},
                0
            ))
            
    def add_life(self, amount: int = 1):
        """Adiciona vidas (para power-ups futuros)"""
        self.lives = min(self.max_lives, self.lives + amount)
        
    def reset(self):
        """Reseta pontuação e vidas"""
        self.score = 0
        self.lives = 3
        self.combo = 0
        self.max_combo = 0
        self.multiplier = 1
        self.reached_milestones.clear()


class EffectsManager:
    """Gerenciador de efeitos visuais e sonoros"""
    
    def __init__(self, event_manager: EventManager):
        self.event_manager = event_manager
        self.active_effects = []
        
        # Inscreve-se aos eventos
        event_manager.subscribe(EventType.COLLISION_BLUE, self._on_collision_blue)
        event_manager.subscribe(EventType.COLLISION_RED, self._on_collision_red)
        event_manager.subscribe(EventType.COMBO_ACHIEVED, self._on_combo)
        event_manager.subscribe(EventType.SCORE_MILESTONE, self._on_milestone)
        
    def _on_collision_blue(self, event: GameEvent):
        """Cria efeito visual para colisão azul"""
        effect = {
            'type': 'particle_burst',
            'position': event.data.get('position'),
            'color': (255, 100, 0),  # Azul
            'duration': 0.5
        }
        self.active_effects.append(effect)
        
    def _on_collision_red(self, event: GameEvent):
        """Cria efeito visual para colisão vermelha"""
        effect = {
            'type': 'screen_shake',
            'intensity': 10,
            'duration': 0.3
        }
        self.active_effects.append(effect)
        
        effect = {
            'type': 'flash',
            'color': (0, 0, 255),  # Vermelho
            'duration': 0.2
        }
        self.active_effects.append(effect)
        
    def _on_combo(self, event: GameEvent):
        """Efeito especial para combo"""
        effect = {
            'type': 'combo_text',
            'text': f"COMBO x{event.data['combo']}!",
            'duration': 1.5
        }
        self.active_effects.append(effect)
        
    def _on_milestone(self, event: GameEvent):
        """Efeito para marco de pontuação"""
        effect = {
            'type': 'achievement',
            'text': f"MILESTONE: {event.data['milestone']} POINTS!",
            'duration': 2.0
        }
        self.active_effects.append(effect)
        
    def update(self, delta_time: float):
        """Atualiza efeitos ativos"""
        # Remove efeitos expirados
        self.active_effects = [e for e in self.active_effects 
                              if e.get('duration', 0) > 0]
        
        # Atualiza duração dos efeitos
        for effect in self.active_effects:
            if 'duration' in effect:
                effect['duration'] -= delta_time
                
    def get_active_effects(self):
        """Retorna lista de efeitos ativos"""
        return self.active_effects.copy()
