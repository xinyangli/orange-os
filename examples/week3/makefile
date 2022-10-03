asmname=$(t)

run :
        touch pm.img
        rm pm.img
        bximage -func=create -fd=1.44M -q pm.img
        mkfs.fat pm.img
        nasm -Dmacro=DEBUG ${asmname}.asm -o ${asmname}.com
        sudo mount -o loop pm.img /mnt/floppy
        sudo cp ${asmname}.com /mnt/floppy
        sudo umount /mnt/floppy
        bochs