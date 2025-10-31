"""
Game Manager - Classe principal que gerencia todos os sistemas do jogo
"""
import cv2
import numpy as np
import time
from typing import Optional, List
from enum import Enum

from game_objects import Wrist, Position
from phase_system import PhaseStrategy, StaticCirclesPhase, MovingCirclesPhase, WavePhase
from event_system import EventManager, ScoreManager, EffectsManager, EventType, GameEvent


class GameState(Enum):
    """Estados do jogo"""
    MENU = "menu"
    PLAYING = "playing"
    PAUSED = "paused"
    GAME_OVER = "game_over"
    PHASE_TRANSITION = "phase_transition"


class GameManager:
    """Gerenciador principal do jogo usando Singleton Pattern"""
    
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(GameManager, cls).__new__(cls)
        return cls._instance
    
    def __init__(self):
        if not hasattr(self, 'initialized'):
            self.initialized = True
            self.screen_width = 1280
            self.screen_height = 720
            
            # Sistema de eventos
            self.event_manager = EventManager()
            self.score_manager = ScoreManager(self.event_manager)
            self.effects_manager = EffectsManager(self.event_manager)
            
            # Punhos
            self.left_wrist = Wrist('left')
            self.right_wrist = Wrist('right')
            
            # Sistema de fases
            self.current_phase: Optional[PhaseStrategy] = None
            self.phase_number = 1
            self.phases = [
                StaticCirclesPhase,
                MovingCirclesPhase,
                WavePhase
            ]
            
            # Estado do jogo
            self.state = GameState.MENU
            self.last_frame_time = time.time()
            self.total_game_time = 0
            self.phase_time = 0
            self.phase_duration = 30  # segundos por fase
            
            # Configurações visuais
            self.show_debug = False
            self.show_effects = True
            
            # Inscrever-se a eventos importantes
            self.event_manager.subscribe(EventType.GAME_OVER, self._on_game_over)
            self.event_manager.subscribe(EventType.PHASE_COMPLETE, self._on_phase_complete)
            
    def start_game(self):
        """Inicia o jogo"""
        self.state = GameState.PLAYING
        self.score_manager.reset()
        self.phase_number = 1
        self.total_game_time = 0
        self.phase_time = 0
        self._load_phase(1)
        
    def _load_phase(self, phase_number: int):
        """Carrega uma fase específica"""
        if phase_number <= len(self.phases):
            phase_class = self.phases[phase_number - 1]
            self.current_phase = phase_class(self.screen_width, self.screen_height)
            self.current_phase.initialize()
            self.phase_time = 0
            self.state = GameState.PLAYING
        else:
            # Jogo completo - volta ao início com dificuldade aumentada
            self.phase_number = 1
            self._load_phase(1)
            
    def update(self, frame: np.ndarray) -> np.ndarray:
        """Atualiza o estado do jogo e retorna o frame renderizado"""
        current_time = time.time()
        delta_time = current_time - self.last_frame_time
        self.last_frame_time = current_time
        
        if self.state == GameState.PLAYING:
            self.total_game_time += delta_time
            self.phase_time += delta_time
            
            # Verifica fim da fase
            if self.phase_time >= self.phase_duration:
                self._complete_phase()
                
            # Atualiza fase atual
            if self.current_phase:
                self.current_phase.update(delta_time)
                
                # Verifica colisões
                self._check_collisions()
                
            # Atualiza efeitos
            self.effects_manager.update(delta_time)
            
        # Renderiza o jogo
        return self._render(frame)
        
    def _check_collisions(self):
        """Verifica colisões entre punhos e círculos"""
        if not self.current_phase:
            return
            
        circles = self.current_phase.get_circles()
        wrists = []
        
        if self.left_wrist.is_detected:
            wrists.append(self.left_wrist)
        if self.right_wrist.is_detected:
            wrists.append(self.right_wrist)
            
        for wrist in wrists:
            for circle in circles:
                if circle.check_collision(wrist.position):
                    # Emite evento de colisão
                    event_type = EventType.COLLISION_BLUE if circle.circle_type.value == "blue" else EventType.COLLISION_RED
                    
                    self.event_manager.emit(GameEvent(
                        event_type,
                        {
                            'position': (circle.position.x, circle.position.y),
                            'points': circle.points,
                            'damage': circle.damage,
                            'wrist': wrist.side
                        },
                        self.total_game_time
                    ))
                    
                    # Remove o círculo
                    self.current_phase.remove_circle(circle)
                    
    def _render(self, frame: np.ndarray) -> np.ndarray:
        """Renderiza todos os elementos do jogo"""
        # Cria overlay para efeitos
        overlay = frame.copy()
        
        if self.state == GameState.MENU:
            self._render_menu(overlay)
            
        elif self.state == GameState.PLAYING:
            # Renderiza círculos
            if self.current_phase:
                for circle in self.current_phase.get_circles():
                    circle.render(overlay)
                    
            # Renderiza punhos
            self.left_wrist.render(overlay)
            self.right_wrist.render(overlay)
            
            # Renderiza HUD
            self._render_hud(overlay)
            
            # Renderiza efeitos
            if self.show_effects:
                self._render_effects(overlay)
                
        elif self.state == GameState.GAME_OVER:
            self._render_game_over(overlay)
            
        elif self.state == GameState.PHASE_TRANSITION:
            self._render_phase_transition(overlay)
            
        return overlay
        
    def _render_hud(self, frame: np.ndarray):
        """Renderiza HUD (pontos, vidas, combo, etc.)"""
        # Fundo semi-transparente para HUD
        hud_bg = frame[10:100, 10:400].copy()
        cv2.rectangle(frame, (10, 10), (400, 100), (0, 0, 0), -1)
        cv2.addWeighted(frame[10:100, 10:400], 0.5, hud_bg, 0.5, 0, frame[10:100, 10:400])
        
        # Pontuação
        cv2.putText(frame, f"Score: {self.score_manager.score}", 
                   (20, 35), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
        
        # Vidas
        lives_text = "♥" * self.score_manager.lives + "♡" * (self.score_manager.max_lives - self.score_manager.lives)
        cv2.putText(frame, f"Lives: {lives_text}", 
                   (20, 65), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
        
        # Combo
        if self.score_manager.combo > 0:
            combo_color = (0, 255, 0) if self.score_manager.combo < 5 else (0, 255, 255) if self.score_manager.combo < 10 else (255, 0, 255)
            cv2.putText(frame, f"Combo: {self.score_manager.combo}x", 
                       (20, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.6, combo_color, 2)
            
        # Multiplicador
        if self.score_manager.multiplier > 1:
            cv2.putText(frame, f"x{self.score_manager.multiplier}", 
                       (250, 35), cv2.FONT_HERSHEY_SIMPLEX, 1.2, (255, 255, 0), 3)
                       
        # Fase e tempo
        remaining_time = max(0, self.phase_duration - self.phase_time)
        cv2.putText(frame, f"Phase {self.phase_number} - {remaining_time:.1f}s", 
                   (self.screen_width - 250, 35), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        
    def _render_effects(self, frame: np.ndarray):
        """Renderiza efeitos visuais"""
        for effect in self.effects_manager.get_active_effects():
            if effect['type'] == 'combo_text':
                # Texto de combo no centro
                text_size = cv2.getTextSize(effect['text'], cv2.FONT_HERSHEY_DUPLEX, 1.5, 3)[0]
                text_x = (frame.shape[1] - text_size[0]) // 2
                text_y = frame.shape[0] // 2 - 100
                
                # Sombra
                cv2.putText(frame, effect['text'], (text_x + 2, text_y + 2), 
                           cv2.FONT_HERSHEY_DUPLEX, 1.5, (0, 0, 0), 5)
                # Texto
                cv2.putText(frame, effect['text'], (text_x, text_y), 
                           cv2.FONT_HERSHEY_DUPLEX, 1.5, (0, 255, 255), 3)
                           
            elif effect['type'] == 'achievement':
                # Texto de conquista
                text_size = cv2.getTextSize(effect['text'], cv2.FONT_HERSHEY_SIMPLEX, 1.0, 2)[0]
                text_x = (frame.shape[1] - text_size[0]) // 2
                text_y = 150
                
                # Fundo
                cv2.rectangle(frame, (text_x - 20, text_y - 30), 
                             (text_x + text_size[0] + 20, text_y + 10), 
                             (0, 100, 200), -1)
                # Texto
                cv2.putText(frame, effect['text'], (text_x, text_y), 
                           cv2.FONT_HERSHEY_SIMPLEX, 1.0, (255, 255, 255), 2)
                           
            elif effect['type'] == 'flash':
                # Flash na tela
                alpha = effect['duration'] / 0.2  # Fade out
                overlay = np.full_like(frame, effect['color'])
                cv2.addWeighted(frame, 1 - alpha * 0.3, overlay, alpha * 0.3, 0, frame)
                
    def _render_menu(self, frame: np.ndarray):
        """Renderiza menu inicial"""
        # Título
        title = "WRIST HUNTER"
        title_size = cv2.getTextSize(title, cv2.FONT_HERSHEY_DUPLEX, 2.5, 3)[0]
        title_x = (frame.shape[1] - title_size[0]) // 2
        title_y = 200
        
        cv2.putText(frame, title, (title_x, title_y), 
                   cv2.FONT_HERSHEY_DUPLEX, 2.5, (0, 255, 255), 3)
        
        # Instruções
        instructions = [
            "Put your wrists in the BLUE circles to score points!",
            "Avoid RED circles - they damage you!",
            "Build combos for multipliers!",
            "",
            "Press SPACE to start",
            "Press Q to quit"
        ]
        
        y = 350
        for instruction in instructions:
            text_size = cv2.getTextSize(instruction, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)[0]
            x = (frame.shape[1] - text_size[0]) // 2
            cv2.putText(frame, instruction, (x, y), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
            y += 40
            
    def _render_game_over(self, frame: np.ndarray):
        """Renderiza tela de game over"""
        # Escurece a tela
        overlay = np.zeros_like(frame)
        cv2.addWeighted(frame, 0.3, overlay, 0.7, 0, frame)
        
        # Game Over
        title = "GAME OVER"
        title_size = cv2.getTextSize(title, cv2.FONT_HERSHEY_DUPLEX, 2.5, 3)[0]
        title_x = (frame.shape[1] - title_size[0]) // 2
        cv2.putText(frame, title, (title_x, 250), 
                   cv2.FONT_HERSHEY_DUPLEX, 2.5, (0, 0, 255), 3)
        
        # Pontuação final
        score_text = f"Final Score: {self.score_manager.score}"
        score_size = cv2.getTextSize(score_text, cv2.FONT_HERSHEY_SIMPLEX, 1.2, 2)[0]
        score_x = (frame.shape[1] - score_size[0]) // 2
        cv2.putText(frame, score_text, (score_x, 350), 
                   cv2.FONT_HERSHEY_SIMPLEX, 1.2, (255, 255, 255), 2)
        
        # Combo máximo
        combo_text = f"Max Combo: {self.score_manager.max_combo}x"
        combo_size = cv2.getTextSize(combo_text, cv2.FONT_HERSHEY_SIMPLEX, 0.9, 2)[0]
        combo_x = (frame.shape[1] - combo_size[0]) // 2
        cv2.putText(frame, combo_text, (combo_x, 400), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 255), 2)
        
        # Instruções
        restart_text = "Press SPACE to play again or Q to quit"
        restart_size = cv2.getTextSize(restart_text, cv2.FONT_HERSHEY_SIMPLEX, 0.7, 2)[0]
        restart_x = (frame.shape[1] - restart_size[0]) // 2
        cv2.putText(frame, restart_text, (restart_x, 500), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        
    def _render_phase_transition(self, frame: np.ndarray):
        """Renderiza transição entre fases"""
        # Título da fase
        title = f"PHASE {self.phase_number} COMPLETE!"
        title_size = cv2.getTextSize(title, cv2.FONT_HERSHEY_DUPLEX, 1.8, 3)[0]
        title_x = (frame.shape[1] - title_size[0]) // 2
        cv2.putText(frame, title, (title_x, 300), 
                   cv2.FONT_HERSHEY_DUPLEX, 1.8, (0, 255, 0), 3)
        
        # Próxima fase
        next_text = f"Next: Phase {self.phase_number + 1}"
        next_size = cv2.getTextSize(next_text, cv2.FONT_HERSHEY_SIMPLEX, 1.0, 2)[0]
        next_x = (frame.shape[1] - next_size[0]) // 2
        cv2.putText(frame, next_text, (next_x, 380), 
                   cv2.FONT_HERSHEY_SIMPLEX, 1.0, (255, 255, 255), 2)
        
    def _complete_phase(self):
        """Completa a fase atual e vai para a próxima"""
        self.state = GameState.PHASE_TRANSITION
        self.event_manager.emit(GameEvent(
            EventType.PHASE_COMPLETE,
            {'phase': self.phase_number, 'score': self.score_manager.score},
            self.total_game_time
        ))
        
        # Agenda próxima fase
        self.phase_number += 1
        # Timer para mostrar transição (seria implementado com threading ou async)
        self._load_phase(self.phase_number)
        
    def _on_game_over(self, event: GameEvent):
        """Callback para game over"""
        self.state = GameState.GAME_OVER
        
    def _on_phase_complete(self, event: GameEvent):
        """Callback para fase completa"""
        # Bonus de vida a cada fase completa
        self.score_manager.add_life(1)
        
    def update_wrist_positions(self, landmarks):
        """Atualiza posições dos punhos com base nos landmarks do MediaPipe"""
        if landmarks:
            import mediapipe as mp
            mp_pose = mp.solutions.pose
            
            # Punho esquerdo
            left_wrist = landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value]
            self.left_wrist.update_position(
                int(left_wrist.x * self.screen_width),
                int(left_wrist.y * self.screen_height)
            )
            
            # Punho direito
            right_wrist = landmarks[mp_pose.PoseLandmark.RIGHT_WRIST.value]
            self.right_wrist.update_position(
                int(right_wrist.x * self.screen_width),
                int(right_wrist.y * self.screen_height)
            )
        else:
            self.left_wrist.is_detected = False
            self.right_wrist.is_detected = False
            
    def handle_key(self, key: int):
        """Processa input do teclado"""
        if key == ord(' '):  # Espaço
            if self.state == GameState.MENU or self.state == GameState.GAME_OVER:
                self.start_game()
            elif self.state == GameState.PLAYING:
                self.state = GameState.PAUSED
            elif self.state == GameState.PAUSED:
                self.state = GameState.PLAYING
                
        elif key == ord('d'):  # Debug
            self.show_debug = not self.show_debug
            
        elif key == ord('e'):  # Efeitos
            self.show_effects = not self.show_effects
