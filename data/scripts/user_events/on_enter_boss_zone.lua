take_player_control_out( true )
save_camera()
load_camera_dummy()
position_camera_to_look_boiler_up()
create_timer("restore_camera", 6.0)
create_timer("after_door_part_opened", 6.0)