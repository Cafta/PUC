"""
Módulo contendo os objetos do jogo (círculos, punhos, etc.)
"""
import cv2
import numpy as np
from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Tuple, Optional, List
from enum import Enum


class CircleType(Enum):
    """Tipos de círculos no jogo"""
    BLUE = "blue"  # Ganha pontos
    RED = "red"    # Perde vida


@dataclass
class Position:
    """Classe para representar posições 2D"""
    x: int
    y: int
    
    def distance_to(self, other: 'Position') -> float:
        """Calcula distância euclidiana entre duas posições"""
        return np.sqrt((self.x - other.x)**2 + (self.y - other.y)**2)

class GameObject(ABC):
    """Classe abstrata base para todos os objetos do jogo"""
    
    def __init__(self, position: Position):
        self.position = position
        self.active = True
    
    @abstractmethod
    def update(self, delta_time: float):
        """Atualiza o estado do objeto"""
        pass
    
    @abstractmethod
    def render(self, frame: np.ndarray):
        """Renderiza o objeto no frame"""
        pass


class Circle(GameObject):
    """Classe representando um círculo no jogo"""
    
    def __init__(self, position: Position, radius: int, circle_type: CircleType):
        super().__init__(position)
        self.radius = radius
        self.circle_type = circle_type
        self.color = self._get_color()
        self.points = 10 if circle_type == CircleType.BLUE else 0
        self.damage = 1 if circle_type == CircleType.RED else 0
        
    def _get_color(self) -> Tuple[int, int, int]:
        """Retorna a cor BGR baseada no tipo do círculo"""
        if self.circle_type == CircleType.BLUE:
            return (255, 100, 0)  # Azul em BGR
        else:
            return (0, 0, 255)  # Vermelho em BGR
    
    def update(self, delta_time: float):
        """Atualização básica (será sobrescrita por subclasses para movimento)"""
        pass
    
    def render(self, frame: np.ndarray):
        """Desenha o círculo no frame"""
        if self.active:
            # Desenha círculo preenchido com transparência
            overlay = frame.copy()
            cv2.circle(overlay, (self.position.x, self.position.y), 
                      self.radius, self.color, -1)
            cv2.addWeighted(overlay, 0.5, frame, 0.5, 0, frame)
            
            # Desenha borda do círculo
            cv2.circle(frame, (self.position.x, self.position.y), 
                      self.radius, self.color, 2)
    
    def check_collision(self, position: Position) -> bool:
        """Verifica se uma posição está dentro do círculo"""
        if not self.active:
            return False
        distance = self.position.distance_to(position)
        return distance <= self.radius


class MovingCircle(Circle):
    """Círculo que se move (para fases mais avançadas)"""
    
    def __init__(self, position: Position, radius: int, circle_type: CircleType,
                 velocity: Tuple[float, float]):
        super().__init__(position, radius, circle_type)
        self.velocity = velocity  # (vx, vy) pixels por segundo
        
    def update(self, delta_time: float):
        """Atualiza a posição do círculo baseado na velocidade"""
        self.position.x += int(self.velocity[0] * delta_time)
        self.position.y += int(self.velocity[1] * delta_time)


class Wrist:
    """Classe representando um punho detectado"""
    
    def __init__(self, side: str):  # 'left' ou 'right'
        self.side = side
        self.position = Position(0, 0)
        self.is_detected = False
        self.trail: List[Position] = []  # Histórico de posições
        self.max_trail_length = 20
        
    def update_position(self, x: int, y: int):
        """Atualiza a posição do punho"""
        self.position = Position(x, y)
        self.is_detected = True
        
        # Adiciona à trilha
        self.trail.append(Position(x, y))
        if len(self.trail) > self.max_trail_length:
            self.trail.pop(0)
            
    def render(self, frame: np.ndarray):
        """Renderiza o punho e sua trilha"""
        if not self.is_detected:
            return
            
        # Desenha a trilha
        for i in range(1, len(self.trail)):
            alpha = i / len(self.trail)  # Fade out gradual
            thickness = int(1 + 3 * alpha)
            color = (0, 255, 0) if self.side == 'left' else (0, 165, 255)  # Verde ou laranja
            cv2.line(frame, 
                    (self.trail[i-1].x, self.trail[i-1].y),
                    (self.trail[i].x, self.trail[i].y),
                    color, thickness)
        
        # Desenha o punho atual
        color = (0, 255, 0) if self.side == 'left' else (0, 165, 255)
        cv2.circle(frame, (self.position.x, self.position.y), 8, color, -1)
        cv2.circle(frame, (self.position.x, self.position.y), 10, (255, 255, 255), 2)
