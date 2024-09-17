import subprocess
EXE_PATH = './space_explorer.exe'

# A script to test the algorithm with different universe generation seeds and report the average hops taken
# to find the treasure and the success rate (the % of seeds for which treasure was found).
# Algorithm efficiency is determined by the average hop rate multiplied by the success rate %.

MAX_SEED = 1000
fail_count = 0
success_count = 0
total_hops = 0

with open('output.txt', 'w') as output_file:
    for i in range(1, MAX_SEED + 1):
        command = [EXE_PATH, str(i)]
        
        try:
            result = subprocess.run(command, capture_output=True, text=True)
            final_line = result.stdout.strip().split('\n')[-1]
            hops = [int(s) for s in final_line.split() if s.isdigit()]
            if len(hops):
              if hops[0] != 0:
                total_hops += hops[0]
              output_file.write(f"SEED: [{i}] | {final_line}\n")
              success_count += 1
              
            else:
              output_file.write(f"SEED: [{i}] | Exceeded max hops. FAILED \n")
              fail_count += 1
            print(f"Processed seed {i}")
        
        except subprocess.CalledProcessError as e:
            print(f"Error executing command with seed {i}: {e}")

print("Execution completed. Output saved to output.txt.")
fail_rate = fail_count / (fail_count + success_count)
print(f"Successes: {success_count}\nFailures: {fail_count}\nSuccess Rate: {100 - (fail_rate * 100):.2f}% ")
print(f"Average hops: {total_hops / success_count:.1f} ")
input()
