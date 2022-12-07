use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

struct FsFile {
	name: String,
	length: u32,
}

struct FsDir {
	name: String,
	dirs: Vec<FsDir>,
  	files: Vec<FsFile>,
}

fn parse_dir(lines: &mut io::Lines<io::BufReader<File>>, curdir: &mut FsDir) -> bool
{
	loop {
		let input: String;

		match lines.next() {
			Some(line) => {
				input = line.expect("Invalid line");
			}
			None => {
				return false;
			}
		}
		
		if input.starts_with("$") {
			if input.starts_with("$ cd ") {
				if input[5..].eq("/") {
					if curdir.name.eq("/") {
						continue;
					} else {
						return false;
					}
				} else if input[5..].eq("..") {
					return true;
				} else {
					match curdir.dirs
						    .iter()
						    .position(|dir| input[5..].eq(&dir.name)) {
						Some(pos) => {
							if parse_dir(lines, &mut curdir.dirs[pos]) == false {
								if curdir.name != "/" {
									return false;
								}
							}
						}
						None => {
							println!("Invalid dir {}", input);
							std::process::exit(1);
						}
					}
				}
			} else if input != "$ ls" {
				println!("Invalid command {}", input);
				std::process::exit(1);
			}
		} else {
			if input.starts_with("dir ") {
				curdir.dirs.push(FsDir {
					name: input[4..].to_string(),
					dirs: Vec::new(),
					files: Vec::new(),
				}); 
			} else {
				let file: Vec<&str> = input.split(" ").collect();
				match file[0].to_string().parse() {
					Ok(length) => {
						curdir.files.push(FsFile {
							name: file[1].to_string(),
							length: length,
						});
					}
					Err(e) => {
						println!("Invalid length format {}", e);
						std::process::exit(1);
					}
				}
			}
		}
	}
}

fn print(curdir: &FsDir, pos: u32)
{
	for _i in 0..pos {
		print!("  ");
	}
	println!("- {} (dir)", curdir.name);

	for dir in &curdir.dirs {
		print(dir, pos + 1);
	}

	for file in &curdir.files {
		for _i in 0..(pos + 1) {
			print!("  ");
		}
		println!("- {} (file, size={})", file.name, file.length);
	}
}

fn dirsize(curdir: &FsDir) -> u32
{
	let mut sum: u32 = 0;

	for dir in &curdir.dirs {
		sum += dirsize(&dir);
	}

	for file in &curdir.files {
		sum += file.length;
	}

	sum
}

fn find_dir_candidate(curdir: &FsDir, sizetofree: u32, candidates: &mut Vec<u32>) -> u32
{
	let mut sum: u32 = 0;

	for dir in &curdir.dirs {
		let size = find_dir_candidate(&dir, sizetofree, candidates);
		sum += size;

		if size >= sizetofree {
			candidates.push(size);
		}
	}

	for file in &curdir.files {
		sum += file.length;
	}

	sum
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut root = FsDir {
		name: String::from("/"),
		dirs: Vec::new(),
		files: Vec::new(),
	};
	let capacity = 70000000;
	let update_size = 30000000;

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	parse_dir(&mut read_lines(&args[1]).expect("Unable to read lines"), &mut root);

	print(&root, 0);

	let cursize = dirsize(&root);

	let mut candidates: Vec<u32> = Vec::new();

	find_dir_candidate(&root, cursize - (capacity - update_size), &mut candidates);

	candidates.sort();

	println!("Best candidate for deletion: {:?}", candidates.first().expect("No suitable candidate found"));
}
