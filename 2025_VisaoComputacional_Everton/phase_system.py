"""
Sistema de fases do jogo usando Strategy Pattern
"""
from abc import ABC, abstractmethod
from typing import List, Tuple
import random
import numpy as np
from game_objects import Circle, MovingCircle, CircleType, Position


class PhaseStrategy(ABC):
    """Interface Strategy para diferentes comportamentos de fase"""
    
    def __init__(self, screen_width: int, screen_height: int):
        self.screen_width = screen_width
        self.screen_height = screen_height
        self.circles: List[Circle] = []
        self.spawn_timer = 0
        self.spawn_interval = 2.0  # segundos entre spawns
        
    @abstractmethod
    def initialize(self):
        """Inicializa a fase"""
        pass
    
    @abstractmethod
    def update(self, delta_time: float):
        """Atualiza a lógica da fase"""
        pass
    
    @abstractmethod
    def spawn_circles(self):
        """Spawna novos círculos de acordo com a lógica da fase"""
        pass
    
    def get_circles(self) -> List[Circle]:
        """Retorna lista de círculos ativos"""
        return [c for c in self.circles if c.active]
    
    def remove_circle(self, circle: Circle):
        """Remove um círculo da lista"""
        circle.active = False
        
    def cleanup_inactive(self):
        """Remove círculos inativos da memória"""
        self.circles = [c for c in self.circles if c.active]


class StaticCirclesPhase(PhaseStrategy):
    """Fase 1: Círculos estáticos"""
    
    def initialize(self):
        """Cria círculos estáticos iniciais"""
        self.spawn_interval = 3.0
        self.max_circles = 5
        self.radius_range = (30, 60)
        
        # Spawna alguns círculos iniciais
        for _ in range(3):
            self.spawn_circles()
            
    def update(self, delta_time: float):
        """Atualiza timer de spawn"""
        self.spawn_timer += delta_time
        
        if self.spawn_timer >= self.spawn_interval:
            if len(self.get_circles()) < self.max_circles:
                self.spawn_circles()
            self.spawn_timer = 0
            
        # Atualiza todos os círculos
        for circle in self.circles:
            circle.update(delta_time)
            
    def spawn_circles(self):
        """Spawna um novo círculo estático em posição aleatória"""
        margin = 80
        x = random.randint(margin, self.screen_width - margin)
        y = random.randint(margin, self.screen_height - margin)
        radius = random.randint(*self.radius_range)
        
        # 70% chance de ser azul (bom), 30% vermelho (ruim)
        circle_type = CircleType.BLUE if random.random() < 0.7 else CircleType.RED
        
        # Verifica sobreposição com círculos existentes
        position = Position(x, y)
        overlapping = False
        for existing_circle in self.get_circles():
            if position.distance_to(existing_circle.position) < radius + existing_circle.radius + 20:
                overlapping = True
                break
                
        if not overlapping:
            circle = Circle(position, radius, circle_type)
            self.circles.append(circle)


class MovingCirclesPhase(PhaseStrategy):
    """Fase 2: Círculos em movimento"""
    
    def initialize(self):
        """Inicializa fase com círculos móveis"""
        self.spawn_interval = 2.5
        self.max_circles = 7
        self.radius_range = (25, 50)
        self.speed_range = (20, 80)  # pixels por segundo
        
        # Spawna alguns círculos iniciais
        for _ in range(4):
            self.spawn_circles()
            
    def update(self, delta_time: float):
        """Atualiza movimento e spawn"""
        self.spawn_timer += delta_time
        
        if self.spawn_timer >= self.spawn_interval:
            if len(self.get_circles()) < self.max_circles:
                self.spawn_circles()
            self.spawn_timer = 0
            
        # Atualiza todos os círculos e verifica limites
        for circle in self.circles:
            circle.update(delta_time)
            
            # Rebate nas bordas
            if isinstance(circle, MovingCircle):
                if circle.position.x <= circle.radius or circle.position.x >= self.screen_width - circle.radius:
                    circle.velocity = (-circle.velocity[0], circle.velocity[1])
                if circle.position.y <= circle.radius or circle.position.y >= self.screen_height - circle.radius:
                    circle.velocity = (circle.velocity[0], -circle.velocity[1])
                    
    def spawn_circles(self):
        """Spawna círculo com movimento aleatório"""
        margin = 80
        x = random.randint(margin, self.screen_width - margin)
        y = random.randint(margin, self.screen_height - margin)
        radius = random.randint(*self.radius_range)
        
        # Velocidade aleatória
        speed = random.randint(*self.speed_range)
        angle = random.uniform(0, 2 * np.pi)
        velocity = (speed * np.cos(angle), speed * np.sin(angle))
        
        # 60% azul, 40% vermelho (mais difícil)
        circle_type = CircleType.BLUE if random.random() < 0.6 else CircleType.RED
        
        position = Position(x, y)
        circle = MovingCircle(position, radius, circle_type, velocity)
        self.circles.append(circle)


class WavePhase(PhaseStrategy):
    """Fase 3: Ondas de círculos com padrões"""
    
    def initialize(self):
        """Inicializa fase de ondas"""
        self.wave_number = 0
        self.wave_timer = 0
        self.wave_interval = 5.0  # segundos entre ondas
        self.radius_range = (35, 45)
        
    def update(self, delta_time: float):
        """Atualiza ondas de círculos"""
        self.wave_timer += delta_time
        
        if self.wave_timer >= self.wave_interval:
            self.spawn_wave()
            self.wave_timer = 0
            self.wave_number += 1
            
        # Atualiza todos os círculos
        for circle in self.circles:
            circle.update(delta_time)
            
        # Remove círculos que saíram da tela
        for circle in self.circles:
            if isinstance(circle, MovingCircle):
                if (circle.position.x < -100 or circle.position.x > self.screen_width + 100 or
                    circle.position.y < -100 or circle.position.y > self.screen_height + 100):
                    circle.active = False
                    
        self.cleanup_inactive()
        
    def spawn_circles(self):
        """Não usado nesta fase - usamos spawn_wave"""
        pass
        
    def spawn_wave(self):
        """Spawna uma onda de círculos com padrão específico"""
        patterns = [
            self._horizontal_wave,
            self._vertical_wave,
            self._diagonal_wave,
            self._circle_formation
        ]
        
        pattern = patterns[self.wave_number % len(patterns)]
        pattern()
        
    def _horizontal_wave(self):
        """Onda horizontal de círculos"""
        y = self.screen_height // 2
        num_circles = 5
        spacing = self.screen_width // (num_circles + 1)
        
        for i in range(num_circles):
            x = spacing * (i + 1)
            radius = random.randint(*self.radius_range)
            circle_type = CircleType.BLUE if i % 2 == 0 else CircleType.RED
            
            # Movimento vertical alternado
            velocity = (0, 50 if i % 2 == 0 else -50)
            circle = MovingCircle(Position(x, y), radius, circle_type, velocity)
            self.circles.append(circle)
            
    def _vertical_wave(self):
        """Onda vertical de círculos"""
        x = self.screen_width // 2
        num_circles = 4
        spacing = self.screen_height // (num_circles + 1)
        
        for i in range(num_circles):
            y = spacing * (i + 1)
            radius = random.randint(*self.radius_range)
            circle_type = CircleType.BLUE if random.random() < 0.6 else CircleType.RED
            
            # Movimento horizontal
            velocity = (60 if i % 2 == 0 else -60, 0)
            circle = MovingCircle(Position(x, y), radius, circle_type, velocity)
            self.circles.append(circle)
            
    def _diagonal_wave(self):
        """Onda diagonal de círculos"""
        num_circles = 6
        for i in range(num_circles):
            x = -50 + i * 30
            y = -50 + i * 30
            radius = random.randint(*self.radius_range)
            circle_type = CircleType.BLUE if i < 4 else CircleType.RED
            
            # Movimento diagonal
            velocity = (70, 70)
            circle = MovingCircle(Position(x, y), radius, circle_type, velocity)
            self.circles.append(circle)
            
    def _circle_formation(self):
        """Formação circular de círculos"""
        center_x = self.screen_width // 2
        center_y = self.screen_height // 2
        radius_formation = 150
        num_circles = 8
        
        for i in range(num_circles):
            angle = (2 * np.pi * i) / num_circles
            x = int(center_x + radius_formation * np.cos(angle))
            y = int(center_y + radius_formation * np.sin(angle))
            radius = random.randint(*self.radius_range)
            
            circle_type = CircleType.BLUE if i % 3 != 0 else CircleType.RED
            
            # Movimento para o centro
            velocity = (-(x - center_x) / 5, -(y - center_y) / 5)
            circle = MovingCircle(Position(x, y), radius, circle_type, velocity)
            self.circles.append(circle)
