qbsp = find_program('qbsp', '../../vendor/ericw-tools-v0.18.1-Linux/bin/qbsp', required: true)
bspinfo = find_program('bspinfo', '../../vendor/ericw-tools-v0.18.1-Linux/bin/bspinfo', required: true)

bsp_names = [
  'test/test',
  ]

if qbsp.found()
  foreach b : bsp_names
    r = run_command(qbsp,
        '-bsp2',
        b + '.map',
        b + '.bsp',
        check : true,
      )

    output = r.stdout().strip()
    message(output)
  endforeach
endif 
