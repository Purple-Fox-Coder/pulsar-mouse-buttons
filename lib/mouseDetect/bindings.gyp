{
    "targets": [
        { "target_name": "mouseDetectLINUX",
            "conditions": [
                ["OS=='linux'", {
                    "include_dirs": ["<!(node -e \"require('nan')\")"],
                    "libraries": ["-lX11"],
                    "sources": ["mouseDetect.cpp"]
                }]
            ]
        },
        {
            "target_name": "mouseDetectWIN",
            "conditions": [
                ["OS=='win'", {
                    "include_dirs": ["<!(node -e \"require('nan')\")"],
                    "libraries": [],
                    "sources": ["mouseDetect.cpp"]
                }]
            ]
        }
    ]
}
