"""
Wrist Hunter Game - Main Entry Point
Um jogo de realidade aumentada usando MediaPipe para detecção de pose
"""
import cv2
import mediapipe as mp
import sys
from game_manager import GameManager


def main():
    """Função principal do jogo"""
    
    # Configuração da câmera
    # ip = "http://192.168.15.34:8080/video"  # Para câmera IP
    
    # Inicializa o módulo de pose do MediaPipe
    mp_pose = mp.solutions.pose
    pose = mp_pose.Pose(
        min_detection_confidence=0.5,
        min_tracking_confidence=0.5
    )
    
    # Inicializa o Game Manager (Singleton)
    game_manager = GameManager()
    
    # Abre a câmera
    cap = cv2.VideoCapture(0)  # Câmera padrão
    # cap = cv2.VideoCapture(ip)  # Para câmera IP
    
    # Define resolução
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, game_manager.screen_width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, game_manager.screen_height)
    
    print("=" * 50)
    print("WRIST HUNTER GAME")
    print("=" * 50)
    print("Instruções:")
    print("- Coloque seus punhos nos círculos AZUIS para ganhar pontos")
    print("- Evite os círculos VERMELHOS - eles causam dano")
    print("- Construa combos para multiplicadores de pontos")
    print("\nControles:")
    print("- ESPAÇO: Iniciar/Pausar jogo")
    print("- D: Toggle modo debug")
    print("- E: Toggle efeitos visuais")
    print("- Q: Sair")
    print("=" * 50)
    
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Erro ao capturar frame da câmera")
            break
        
        # Espelha o frame horizontalmente para experiência mais natural
        frame = cv2.flip(frame, 1)
        
        # Converte para RGB para o MediaPipe
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        
        # Processa a pose
        results = pose.process(rgb_frame)
        
        # Atualiza posições dos punhos no game manager
        if results.pose_landmarks:
            game_manager.update_wrist_positions(results.pose_landmarks.landmark)
        else:
            game_manager.update_wrist_positions(None)
        
        # Atualiza e renderiza o jogo
        frame = game_manager.update(frame)
        
        # Mostra informações de debug se ativado
        if game_manager.show_debug:
            render_debug_info(frame, results, game_manager)
        
        # Exibe o frame
        cv2.imshow('Wrist Hunter Game', frame)
        
        # Processa input do teclado
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
        elif key != 255:  # Alguma tecla foi pressionada
            game_manager.handle_key(key)
    
    # Limpeza
    cap.release()
    cv2.destroyAllWindows()
    pose.close()
    
    # Mostra estatísticas finais
    print("\n" + "=" * 50)
    print("GAME OVER - Estatísticas Finais")
    print("=" * 50)
    print(f"Pontuação Final: {game_manager.score_manager.score}")
    print(f"Combo Máximo: {game_manager.score_manager.max_combo}x")
    print(f"Fase Alcançada: {game_manager.phase_number}")
    print(f"Tempo Total: {game_manager.total_game_time:.1f}s")
    print("=" * 50)
    print("Obrigado por jogar!")


def render_debug_info(frame, pose_results, game_manager):
    """Renderiza informações de debug"""
    debug_y = 150
    debug_color = (0, 255, 0)
    
    # FPS (aproximado)
    cv2.putText(frame, f"State: {game_manager.state.value}", 
               (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, debug_color, 1)
    debug_y += 20
    
    # Detecção de pose
    pose_detected = "Yes" if pose_results.pose_landmarks else "No"
    cv2.putText(frame, f"Pose Detected: {pose_detected}", 
               (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, debug_color, 1)
    debug_y += 20
    
    # Punhos detectados
    left_detected = "Yes" if game_manager.left_wrist.is_detected else "No"
    right_detected = "Yes" if game_manager.right_wrist.is_detected else "No"
    cv2.putText(frame, f"Wrists - L: {left_detected}, R: {right_detected}", 
               (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, debug_color, 1)
    debug_y += 20
    
    # Número de círculos ativos
    if game_manager.current_phase:
        num_circles = len(game_manager.current_phase.get_circles())
        cv2.putText(frame, f"Active Circles: {num_circles}", 
                   (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, debug_color, 1)
        debug_y += 20
    
    # Eventos recentes
    recent_events = game_manager.event_manager.get_history()[-3:]
    cv2.putText(frame, "Recent Events:", 
               (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, debug_color, 1)
    debug_y += 20
    
    for event in recent_events:
        event_text = f"  - {event.event_type.value}"
        cv2.putText(frame, event_text, 
                   (10, debug_y), cv2.FONT_HERSHEY_SIMPLEX, 0.4, debug_color, 1)
        debug_y += 15
    
    # Desenha skeleton completo se detectado
    if pose_results.pose_landmarks:
        mp_drawing = mp.solutions.drawing_utils
        mp_drawing.draw_landmarks(
            frame,
            pose_results.pose_landmarks,
            mp.solutions.pose.POSE_CONNECTIONS,
            landmark_drawing_spec=mp_drawing.DrawingSpec(
                color=(0, 255, 0), thickness=1, circle_radius=2),
            connection_drawing_spec=mp_drawing.DrawingSpec(
                color=(0, 255, 0), thickness=1)
        )


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nJogo interrompido pelo usuário")
        sys.exit(0)
    except Exception as e:
        print(f"\nErro no jogo: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
