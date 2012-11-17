{
    "targets": [
        {
            "target_name": "toimage",
            "sources": ["src/toimage.cc"],
            "include_dirs": ["/usr/local/include/imageexport"],
            "link_settings": {
                "libraries": ["-lsc_ex", "-lsc_da"],
                "ldflags": ["-L/usr/local/lib/imageexport"]
            }
        }
    ]
}