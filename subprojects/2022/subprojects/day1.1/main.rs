use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	let file = File::open(filename)?;
	Ok(io::BufReader::new(file).lines())
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut v: Vec<i32> = Vec::new();
	let mut sum: i32 = 0;

	if let Ok(lines) = read_lines(&args[1]) {
		for line in lines {
			if let Ok(ip) = line {
				if ip == "" {
					v.push(sum);
					sum = 0;
				} else {
					let parsed: i32 = ip.parse().unwrap();
					sum += parsed;
				}
			}
		}
		v.push(sum);
	}

	v.sort();
	v.reverse();

	println!("{}", v[0] + v[1] + v[2]);
}
