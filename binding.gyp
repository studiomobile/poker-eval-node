{
  "targets": [
    {
      "target_name": "wrapper",
      "sources": [
        "src/wrapper.cc"
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