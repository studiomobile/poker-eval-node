{
  "targets": [
    {
      "target_name": "wrapper",
      "sources": [
        "src/wrapper.cc",
        "src/game.cc",
        "src/game_eval.cc",
        "src/game_enum.cc",
        "src/hand_enum.cc",
        "src/enum.c"
      ],
      "include_dirs": [
        "/usr/local/include",
        "/usr/local/include/poker-eval",
        "/usr/include/poker-eval"
      ],
      "libraries": [
        "<!@(pkg-config --libs poker-eval)",
      ]
    }
  ]
}