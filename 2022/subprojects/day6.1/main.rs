use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str, count: usize)
{
	assert!(line.len() >= count);
	for i in 0..(line.len() - count) {
		let mut matched = false;
		for a in i..(i + count) {
			for b in (a + 1)..(i + count) {
				if line.chars().skip(a).take(1).eq(line.chars().skip(b).take(1)) {
					matched = true;
				}
			}
		}

		if matched == false {
			println!("{}", i + count);
			return;
		}
	}
}

fn main() {
	let args: Vec<String> = env::args().collect();

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		parse_line(&line.expect("Invalid line"), 14);
	}
}
