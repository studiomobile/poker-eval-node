{
  "targets": [
    {
      "target_name": "wrapper",
      "sources": [
        "src/wrapper.cc",
        "src/eval_cards.cc",
        "src/game_enum.cc"
      ],
      "include_dirs": [
        "/usr/local/include/poker-eval",
        "/usr/include/poker-eval"
      ],
      "libraries": [
        "<!@(pkg-config --libs poker-eval)",
      ]
    }
  ]
}