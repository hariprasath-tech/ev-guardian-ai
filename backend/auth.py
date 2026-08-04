import time
import jwt
from fastapi import HTTPException, Security, Depends
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials

SECRET_KEY = "safecell_ai_super_secret_jwt_key_2026"
ALGORITHM = "HS256"

security = HTTPBearer(auto_error=False)

# Dummy User Registry
USERS = {
    "driver": {"password": "password123", "role": "Driver", "name": "Alex Chen"},
    "engineer": {"password": "password123", "role": "Engineer", "name": "Dr. Sarah Lin"},
    "admin": {"password": "adminpassword", "role": "Administrator", "name": "System Admin"}
}

def create_access_token(data: dict, expires_delta: int = 86400):
    to_encode = data.copy()
    expire = time.time() + expires_delta
    to_encode.update({"exp": expire})
    encoded_jwt = jwt.encode(to_encode, SECRET_KEY, algorithm=ALGORITHM)
    return encoded_jwt

def decode_access_token(token: str):
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        return payload
    except jwt.PyJWTError:
        raise HTTPException(status_code=401, detail="Invalid or expired authentication token")

def get_current_user(credentials: HTTPAuthorizationCredentials = Depends(security)):
    if not credentials:
        # Default fallback for prototype UI
        return {"sub": "driver", "role": "Driver", "name": "Alex Chen"}
    token = credentials.credentials
    return decode_access_token(token)

def require_role(required_role: str):
    def role_checker(user: dict = Depends(get_current_user)):
        roles_hierarchy = {"Driver": 1, "Engineer": 2, "Administrator": 3}
        user_role = user.get("role", "Driver")
        if roles_hierarchy.get(user_role, 0) < roles_hierarchy.get(required_role, 1):
            raise HTTPException(status_code=403, detail=f"Permission denied: Requires {required_role} role")
        return user
    return role_checker
