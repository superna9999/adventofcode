use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn line_contains(c: char, _line: &Result<String, std::io::Error>) -> bool
{
	if let Ok(line) = _line {
		return line.contains(c);
	}

	false
}

fn parse_group(_group: &[Result<String, std::io::Error>]) -> u8
{
	assert!(_group.len() == 3);

	if let Ok(first) = &_group[0] {
		for c in first.chars() {
			if line_contains(c, &_group[1]) && line_contains(c, &_group[2]) {
				let common = c as u8;
				if common >= b'A' && common <= b'Z' {
					return common - b'A' + 27;
				} else {
					return common - b'a' + 1;
				}
			}
		}
	}

	0
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut sum: u32 = 0;
	let mut group: Vec<Result<String, std::io::Error>> = Vec::new();
	let mut n = 0;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	let lines = read_lines(&args[1]).expect("Unable to read lines");

	for line in lines {
		group.push(line);
	}

	assert!(group.len() % 3 == 0);

	while n < group.len() {
		sum += parse_group(&group[n..(n+3)]) as u32;

		n += 3;
	}

	println!("{}", sum);
}
