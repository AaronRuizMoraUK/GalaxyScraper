enable_collisions(true)
player_win(false)
player_lose(false)
player_lose_timeout(false)
enable_samples(true)
--stop_ambient()
play_ambient("normal")
delete_timer("iuranus_planet_timeout")
create_timer("iuranus_planet_timeout", 240)
reset_iuranus_logic_counters()
place_player_initial_position()
place_camera_initial_position()
reset_iuranus_enemies_states()
take_player_control_out(false)
create_enemy()