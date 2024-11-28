.global invalidate_page_dir
invalidate_page_dir:
    mov 4(%esp), %eax
    invlpg (%eax)
    ret
    