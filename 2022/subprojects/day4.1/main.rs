use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str) -> bool
{
	assert!(line.contains(","));
	let t: Vec<&str> = line.split(",").collect();
	assert!(t[0].contains("-"));
	assert!(t[1].contains("-"));
	let t1: Vec<&str> = t[0].split("-").collect();
	let t2: Vec<&str> = t[1].split("-").collect();
	let t1s: u32 = t1[0].parse().unwrap();
	let t1e: u32 = t1[1].parse().unwrap();
	let t2s: u32 = t2[0].parse().unwrap();
	let t2e: u32 = t2[1].parse().unwrap();

	if ((t2s >= t1s && t2s <= t1e) || (t2e >= t1s && t2e <= t1e)) ||
	   ((t1s >= t2s && t1s <= t2e) || (t1e >= t2s && t1e <= t2e)) {
		   return true;
	}

	false
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut num = 0;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		if parse_line(&line.expect("Invalid line")) {
			num += 1;
		}
	}

	println!("{}", num);
}
