## Project
This project is designed to analyze the probability of vehicle accidents using computer vision.

## TEAM
- 김규진 (Kim Gyu Jin) [Github](https://github.com/garfield0xff) email : garfield0xff@gmail.com
- 김한빈 (Kim Han bin) [Github](https://github.com/hanbeen00)
email : hanbini2008@naver.com
- 김태훈 (Kim Tae hun) [Github](http://github.com/aksmf98)
email : aksmf98@naver.com
- 장윤재 (Jang Yun Jae) [Github](http://github.com/mealsOrder)
email : dbsw7007@gmail.com
- 조세기 (Jo Segi) [Github](https://github.com/orgs/HV-Tech-Corporation/people/SegiJo)
email : jsg0423@naver.com

## Features

Depending on our Monitoring System use case, you might need to know what features are included 

| Feature | Description |
|---------|--------------|
| BestShot |  Caputre Image by track ID ( YOLO, SORT )
| OCR | Vehicle plate license ( OpenCV, PaddleOCR ) |
| Accident Cost | A compatible control plane and (optionally) an NGINX One SaaS console data plane token |



## Accident Cost Algorithm
- Our project aims to visualize 


## Installation


### Package  (Required)

- [TVM](https://github.com/apache/tvm) 

```bash
git clone --recursive https://github.com/apache/tvm.git
cd tvm
git submodule init
git submodule update --recursive
mkdir -p build
cd build



```
- [OpenCV](https://github.com/opencv/opencv) 
- [Boost](https://github.com/boostorg/boost) 

#### Ansible Lint (Optional)

- Ansible Lint is used to lint the role for both Ansible best practices and potential Ansible/YAML issues.
- Instructions on how to install Ansible Lint can be found in the [Ansible Lint website](https://ansible.readthedocs.io/projects/lint/installing/).
- Once installed, using Ansible Lint is as easy as running:

  ```bash
  ansible-lint
  ```

- For ease of use, you can install and/or upgrade Ansible Lint by running the following command on your Ansible host:

  ```bash
  pip install -r https://raw.githubusercontent.com/nginxinc/ansible-role-nginx/main/.github/workflows/requirements/requirements_ansible_lint.txt
  ```

#### Molecule (Optional)

- Molecule is used to test the various functionalities of the role.
- Instructions on how to install Molecule can be found in the [Molecule website](https://molecule.readthedocs.io/en/latest/installation.html). *You will also need to install the Molecule plugins package and the Docker Python SDK.*
- To run any of the NGINX Plus Molecule tests, you must first copy your NGINX Plus license to the role's [`files/license`](/files/license/) directory.







