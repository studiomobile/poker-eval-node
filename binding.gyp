{
  "targets": [
    {
      "target_name": "wrapper",
      "sources": [
        "src/wrapper.cc"
      ],
      "include_dirs": [
        "/usr/local/include/poker-eval"
      ],
      "libraries": [
        "<!@(pkg-config --libs poker-eval)",
      ]
    }
  ]
}