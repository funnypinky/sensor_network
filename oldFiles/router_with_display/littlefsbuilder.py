Import("env")
env.Replace( MKSPIFFSTOOL=env.get("PROJECT_DIR") + '/mkfatfs_linux_x64' )