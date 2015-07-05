import os
import subprocess

compilation_db = "--"
executable = "./ttta"
input_dir = "./inputs/"

for filename in os.listdir(input_dir):
  # Invoke TypedefToTypeAlias
  p = subprocess.Popen([executable + " " + input_dir + filename + " " + compilation_db],
          stdout=subprocess.PIPE, 
          stderr=subprocess.PIPE,
          shell=True)

  (out, err) = p.communicate()

  if err:
      print "Error on file: %s" % filename
      print err
      continue

  # Compare output to expected results
  outputs = out.split('\n')
  all_pass = True

  for actual_result, expected_result in zip(outputs[0::2], outputs[1::2]):
    expected_result = expected_result.replace("// ", "")
    if not actual_result == expected_result:
      print "Unexpected result in %s" % filename
      print "Expected: %s" % expected_result
      print "Actual: %s" % actual_result
      all_pass = False

  if all_pass:
    print "Tests in %s have all passed" % filename

