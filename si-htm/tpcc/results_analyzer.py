import sys,os
import numpy 

avail_threads = [1,2,4,8,16,32]

run_name = sys.argv[1]
outputs_folder = sys.argv[2]

files = [os.path.join(outputs_folder,f) for f in os.listdir(outputs_folder) if f.startswith(run_name+"-") and f.endswith("data")]# and len(f.split(run_name)[1].split("-"))==3 and f.split(run_name)[1].split("-")[0]==""]


runs = {}
for f in files:
	try:
		threads = int(os.path.basename(f).split(run_name)[1].split("-")[1])
	except:
		sys.stderr.write(f+"\n")
		continue
	if threads not in runs:
		runs[threads] = {"time":[],"commits":[],"aborts":[],"htm_commits":[],"rot_commits":[],"gl_commits":[],"htm_conflicts":[],"htm_self":[],"htm_trans":[],"htm_nontrans":[],"htm_user":[],"htm_capacity":[],"htm_persistent":[],"rot_conflicts":[],"rot_self":[],"rot_trans":[],"rot_nontrans":[],"rot_user":[],"rot_capacity":[],"rot_persistent":[],"read_commits":[],"htm_other":[],"rot_other":[],"stm_commits":[],"stm_aborts":[],"readers_aborts":[],"latency_r":[],"latency_w":[]}
	fn = f
	f = open(f)
	time = -1
	commits = 0
	aborts = 0
	htm_commits = 0
	rot_commits = 0
	gl_commits = 0
	read_commits = 0
	rot_conflicts = 0
	rot_self = 0
	rot_user = 0
	rot_capacity = 0
	rot_trans = 0
	rot_nontrans = 0
	rot_persistent = 0
	rot_other = 0
	htm_conflicts = 0
        htm_self = 0
        htm_user = 0
        htm_capacity = 0
        htm_trans = 0
        htm_nontrans = 0
        htm_persistent = 0
	htm_other = 0
	readers_aborts = 0
	stm_commits = 0
	stm_aborts = 0
	latency_r = 0
	latency_w = 0
	for line in f:
		line = line.strip().lower()
		if "prefix" in line:
			continue
		if "total time" in line:
			time = float(line.split(" ")[-1])
		elif "read commits" in line:
			read_commits = float(line.split(" ")[-1])
		elif "total commits" in line:
			commits = float(line.split(" ")[-1])
		elif "starts" in line:
			commits = float(line.split("=")[1].split(" ")[0])
			aborts = float(line.split("=")[-1])
		elif "total aborts" in line:
			aborts = float(line.split(" ")[-1])
		elif "htm commits" in line:
			htm_commits = float(line.split(" ")[-1])
		elif "conflict aborts" in line and "rot" not in line:
			htm_conflicts = float(line.split(" ")[-1])
		elif "user aborts" in line and "rot" not in line:
			htm_user = float(line.split(" ")[-1])
		elif "self aborts" in line and "rot" not in line:
			htm_self = float(line.split(" ")[-1])
		elif "capacity aborts" in line and "rot" not in line:
			htm_capacity = float(line.split(" ")[-1])
		elif "persistent aborts" in line and "rot" not in line:
			htm_persistent = float(line.split(" ")[-1])
		elif "htm trans" in line:
			htm_trans = float(line.split(" ")[-1])
		elif "htm non-trans" in line:
			htm_nontrans = float(line.split(" ")[-1])
		elif "other aborts" in line and "rot" not in line:
			htm_other = float(line.split(" ")[-1])
		elif "readers aborts" in line and "rot" not in line:
			readers_aborts = float(line.split(" ")[-1])
		elif "rot commits" in line:
			rot_commits = float(line.split(" ")[-1])
		elif "rot conflict aborts" in line:
                        rot_conflicts = float(line.split(" ")[-1])
                elif "rot user aborts" in line:
                        rot_user = float(line.split(" ")[-1])
                elif "rot self aborts" in line:
                        rot_self = float(line.split(" ")[-1])
                elif "rot capacity aborts" in line:
                        rot_capacity = float(line.split(" ")[-1])
                elif "rot persistent aborts" in line:
                        rot_persistent = float(line.split(" ")[-1])
                elif "rot trans" in line:
                        rot_trans = float(line.split(" ")[-1])
                elif "rot non-trans" in line:
                        rot_nontrans = float(line.split(" ")[-1])
                elif "rot other aborts" in line:
                        rot_other = float(line.split(" ")[-1])
		elif "gl commits" in line:
			gl_commits = float(line.split(" ")[-1])
		elif "stm commits" in line:
                        stm_commits = float(line.split(" ")[-1])
		elif "stm aborts" in line:
                        stm_aborts = float(line.split(" ")[-1])
		elif "latency reader" in line:
			latency_r = float(line.split(" ")[-1])
		elif "latency writer" in line:
			latency_w = float(line.split(" ")[-1])
	f.close()
	if time == -1:
		continue
	else:
		runs[threads]["time"].append(time)
		runs[threads]["commits"].append(commits)
		runs[threads]["htm_commits"].append(htm_commits)
		runs[threads]["rot_commits"].append(rot_commits)
		runs[threads]["read_commits"].append(read_commits)
		runs[threads]["gl_commits"].append(gl_commits)
		runs[threads]["aborts"].append(aborts)
		runs[threads]["htm_conflicts"].append(htm_conflicts)
		runs[threads]["htm_self"].append(htm_self)
		runs[threads]["htm_user"].append(htm_user)
		runs[threads]["htm_capacity"].append(htm_capacity)
		runs[threads]["htm_trans"].append(htm_trans)
		runs[threads]["htm_nontrans"].append(htm_nontrans)
		runs[threads]["htm_persistent"].append(htm_persistent)
		runs[threads]["htm_other"].append(htm_other)
		runs[threads]["readers_aborts"].append(readers_aborts)
		runs[threads]["rot_conflicts"].append(rot_conflicts)
                runs[threads]["rot_self"].append(rot_self)
                runs[threads]["rot_user"].append(rot_user)
                runs[threads]["rot_capacity"].append(rot_capacity)
                runs[threads]["rot_trans"].append(rot_trans)
                runs[threads]["rot_nontrans"].append(rot_nontrans)
                runs[threads]["rot_persistent"].append(rot_persistent)
                runs[threads]["rot_other"].append(rot_other)
		runs[threads]["stm_commits"].append(stm_commits)
		runs[threads]["stm_aborts"].append(stm_aborts)
		runs[threads]["latency_r"].append(latency_r)
		runs[threads]["latency_w"].append(latency_w)

print "threads\ttime\ttime_std\tcommits_avg\tcommits_std\tThroughput_avg\tThroughput_std\thtm_commits_avg\thtm_commits_std\trot_commits_avg\trot_commits_std\tgl_commits_avg\tgl_commits_std\tread_commits_avg\tread_commits_std\taborts_avg\taborts_std\thtm_conflicts_avg\thtm_conflicts_std\thtm_self_avg\thtm_self_std\thtm_trans_avg\thtm_trans_std\thtm_nontrans_avg\thtm_nontrans_std\thtm_user_avg\thtm_user_std\thtm_capacity_avg\thtm_capacity_std\thtm_persistent_avg\thtm_persistent_std\thtm_other_avg\thtm_other_std\trot_conflicts_avg\trot_conflicts_std\trot_self_avg\trot_self_std\trot_trans_avg\trot_trans_std\trot_nontrans_avg\trot_nontrans_std\trot_user_avg\trot_user_std\trot_capacity_avg\trot_capacity_std\trot_persistent_avg\trot_persistent_std\trot_other_avg\trot_other_std\tstm_commits_avg\tstm_commits_std\tstm_aborts_avg\tstm_aborts_std\treaders_aborts_avg\treaders_aborts_std\tlatency_r_avg\tlatency_r_std\tlatency_w_avg\tlatency_r_std"
for k in avail_threads:
	out = str(k)
	if k not in runs:
		print str(k)+"\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0\t0"
		continue
        v = runs[k]
        time_avg = numpy.average(v["time"])
        time_std =  numpy.std(v["time"])
        if time_std/time_avg * 100 > 10:
                sys.stderr.write(str(k) + ", time, " + str(time_std/time_avg) + "\n")
        out += "\t"+str(time_avg)
        out += "\t"+str(time_std)
	commits_avg = numpy.average(v["commits"])
	std = numpy.std(v["commits"])
	if std/commits_avg * 100 > 10:
		sys.stderr.write(str(k) + ", commits, " + str(std/commits_avg) + "\n")
	out += "\t"+str(commits_avg)
	out += "\t"+str(std)

        c=numpy.array(v["commits"])
        t=numpy.array(v["time"])   
        avg=numpy.average(c/t)
        std=numpy.std(c/t)
        out += "\t"+str(avg)
        out += "\t"+str(std)

        avg = numpy.average(v["htm_commits"])
        std =  numpy.std(v["htm_commits"])
        out += "\t"+str(avg/(commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_commits"])
        std =  numpy.std(v["rot_commits"])
        out += "\t"+str(avg/(commits_avg)*100)
        out += "\t"+str(std)
        gl_commits_avg = numpy.average(v["gl_commits"])
	stm_commits_avg = numpy.average(v["stm_commits"])
	if(gl_commits_avg != 0):
	        std =  numpy.std(v["gl_commits"])
        	out += "\t"+str(gl_commits_avg/(commits_avg)*100)
	else:
		std =  numpy.std(v["stm_commits"])
                out += "\t"+str(stm_commits_avg/(commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["read_commits"])
        std =  numpy.std(v["read_commits"])
        out += "\t"+str(avg/(commits_avg)*100)
        out += "\t"+str(std)
        aborts_avg = numpy.average(v["aborts"])
        std =  numpy.std(v["aborts"])
        out += "\t"+str(aborts_avg)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_conflicts"])
        std =  numpy.std(v["htm_conflicts"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_self"])
	std =  numpy.std(v["htm_self"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_trans"])
        std =  numpy.std(v["htm_trans"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_nontrans"])
        std =  numpy.std(v["htm_nontrans"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_user"])
        std =  numpy.std(v["htm_user"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_capacity"])
        std =  numpy.std(v["htm_capacity"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_persistent"])
        std =  numpy.std(v["htm_persistent"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["htm_other"])
        std =  numpy.std(v["htm_other"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_conflicts"])
        std =  numpy.std(v["rot_conflicts"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_self"])
        std =  numpy.std(v["rot_self"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_trans"])
        std =  numpy.std(v["rot_trans"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_nontrans"])
        std =  numpy.std(v["rot_nontrans"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_user"])
        std =  numpy.std(v["rot_user"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_capacity"])
        std =  numpy.std(v["rot_capacity"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_persistent"])
        std =  numpy.std(v["rot_persistent"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["rot_other"])
        std =  numpy.std(v["rot_other"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
	avg = numpy.average(v["stm_commits"])
        std =  numpy.std(v["stm_commits"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
        avg = numpy.average(v["stm_aborts"])
        std =  numpy.std(v["stm_aborts"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
	avg = numpy.average(v["readers_aborts"])
        std =  numpy.std(v["readers_aborts"])
        out += "\t"+str(avg/(aborts_avg+commits_avg)*100)
        out += "\t"+str(std)
	avg = numpy.average(v["latency_r"])
        std =  numpy.std(v["latency_r"])
        out += "\t"+str(avg)
        out += "\t"+str(std)
	avg = numpy.average(v["latency_w"])
        std =  numpy.std(v["latency_w"])
        out += "\t"+str(avg)
        out += "\t"+str(std)
        print out.replace("nan","0")
