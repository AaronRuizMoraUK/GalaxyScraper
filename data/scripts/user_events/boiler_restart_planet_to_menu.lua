-- reset stuff
set_fallen_condition(false)
enable_player_flicker(false)
reset_logic()
reset_checkpoints()

-- init part
enable_collisions(true)
player_win(false)
player_lose(false)
player_lose_timeout(false)
enable_samples(false)
stop_ambient()
--place_player_initial_position()
--place_camera_initial_position()
--delete_timer("boiler_planet_timeout")
--create_timer("boiler_planet_timeout", 240)
delete_timer("flame_thrower_ended")
delete_timer("lava_with_bubbles_ended")
--stop_boss_fires
delete_timer("boiler_planet_timeout")